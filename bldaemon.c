/***********************************************************************
 * Backlight daemon for TinyPi Pro 2019-05-06
 *
 * 2019-05-05 mozzwald	make it work for the TinyPi Pro.
 *			only handles the screen backlight
 * 2019-05-06 mozzwald	switch to bcm2835 gpio library. the previously
 *			used pigpio lib was causing issues with the
 *			sound and the system seemed to run slow
 *
 ***********************************************************************/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include <pthread.h>

#include <time.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>

#include <bcm2835.h>

pthread_t get_keypressed[1];
int wasKeyPressed = 0;
pthread_mutex_t lock;
static int evfd;

typedef unsigned long u32;

#define CLOCKID CLOCK_MONOTONIC
#define SIG SIGALRM
#define LCD_TIMER  		201
#define LCD_TIMEOUT  	2500 // 6000 = 60 secs

#define BL_PIN RPI_GPIO_P1_15 // Pin 15 is GPIO 22

/* pthread function to watch for keypresses */
void* GetKeyPressed(void *arg) {
	ssize_t n;
	struct input_event ev;

	while (1) {
		n = read(evfd, &ev, sizeof ev);
		if (n == (ssize_t)-1) {
			if (errno == EINTR)
				continue;
			else
				break;
	        } else {
			if (n != sizeof ev) {
				errno = EIO;
				break;
			}
			if (ev.type == EV_KEY && ev.value >= 0 && ev.value <= 2){
				pthread_mutex_lock(&lock);
				wasKeyPressed = 1;
				pthread_mutex_unlock(&lock);
				//printf("%s 0x%04x (%d)\n", evval[ev.value], (int)ev.code, (int)ev.code);
			}
		}
	}
	fflush(stdout);
	fprintf(stderr, "%s.\n", strerror(errno));

	return NULL;
}

static timer_t keys_timerid = 0;
static timer_t lcd_timerid = 0;
static timer_t power_timerid = 0;

static unsigned int bScreenOff = 0;

static inline void screenOn(){
	sigset_t mask;
	/* Block timer signal temporarily */
	sigemptyset(&mask);
	sigaddset(&mask, SIG);
	if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
		perror("sigprocmask");

	if(bScreenOff){ //turn it back on
		bcm2835_gpio_write(BL_PIN, LOW); // LOW is ON
		bScreenOff = 0;
	}

	/* Unlock the timer signal, so that timer notification nan be delivered */
	if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
		perror("sigprocmask");
}

static inline void screenOff(){
	bcm2835_gpio_write(BL_PIN, HIGH); // HIGH is OFF
	bScreenOff = 1;
}

static void onTimer(int sig, siginfo_t *si, void *uc)
{
	switch(si->si_int){
		case LCD_TIMER:
			screenOff();
			break;
		default:
			break;
	}
}

#define errExit(msg)    do { perror(msg); return 0; \
                        } while (0)

timer_t create_timer(int timerName, unsigned int freq_msecs)
{
	struct itimerspec 	its;
						its.it_value.tv_sec = freq_msecs / 100;
						its.it_value.tv_nsec = 0;
						its.it_interval.tv_sec = 0;
						its.it_interval.tv_nsec = 0;

	struct sigevent 	sev;
						sev.sigev_notify = SIGEV_SIGNAL;
						sev.sigev_signo = SIG;
						sev.sigev_value.sival_int = timerName;

	struct sigaction 	sa;
						sa.sa_flags = SA_SIGINFO;
						sa.sa_sigaction = onTimer;

	timer_t timerid=0;

	/* Establish handler for timer signal */
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIG, &sa, NULL) == -1)
		errExit("sigaction");

	/* Create the timer */
	if (timer_create(CLOCKID, &sev, &timerid) == -1)
		errExit("timer_create");

	/* Start the timer */
	if (timer_settime(timerid, 0, &its, NULL) == -1)
		errExit("timer_settime");

	return timerid;
}

static int set_timer(timer_t timerid, unsigned int freq_msecs)
{
	struct itimerspec 	its;
						its.it_value.tv_sec = freq_msecs / 100;
						its.it_value.tv_nsec = 0;
						its.it_interval.tv_sec = 0;
						its.it_interval.tv_nsec = 0;

	/* Start the timer */
	if (timer_settime(timerid, 0, &its, NULL) == -1)
		errExit("timer_settime");

	return 1;
}

int main(int argc, char **argv) {
	/* Setup Pi GPIO */
	if (!bcm2835_init()){
		fprintf(stderr, "bcm2835 gpio initialisation failed\n");
		return 1;
	}

	bcm2835_gpio_fsel(BL_PIN, BCM2835_GPIO_FSEL_OUTP);

	/* Setup the LCD Timer */
	lcd_timerid = create_timer(LCD_TIMER, LCD_TIMEOUT);

	/* open input event device */
	const char *evdev = "/dev/input/event0";
	evfd = open(evdev, O_RDONLY);
	if (evfd == -1) {
		fprintf(stderr, "Cannot open %s: %s.\n", evdev, strerror(errno));
		return EXIT_FAILURE;
	}

	/* setup keypressed pthread */
	pthread_mutex_init(&lock, NULL);
	pthread_create(&(get_keypressed[1]), NULL, &GetKeyPressed, NULL);

	/* Main Loop */
	while(1) {
		if( wasKeyPressed == 1)
		{
			/* a key has been pressed -- reset the timer */
			set_timer(lcd_timerid, LCD_TIMEOUT);
			wasKeyPressed = 0;
			/* turn on the backlight */
			screenOn();
		}

		sleep(1); /* wait one second */
	}
}









