#define NUM_SECONDS  2
#define SAMPLE_RATE  44100
/*
 * 1. Record audio to a buffer.
 * 2. Save buffer to disk.
 * 3. Read buffer from disk.
 * 4. Play audio.
 */

struct recording {
    int len, cap;
    float *data;
};

static int icallback(
  const void *input,
  void *output,
  unsigned long nframes,
  const PaStreamCallbackTimeInfo* timeinfo,
  PaStreamCallbackFlags flags,
  void *userdata)
{
	struct recording *ud = userdata;
	int status;

	(void) output;
	(void) timeinfo;
	(void) flags;

	unsigned rem = ud->cap - ud->len;
	if (rem < nframes) {
		nframes = rem;
		status = paComplete;
	} else {
		status = paContinue;
	}

	float *wp = &ud->data[ud->len];
	const float *rp = input;
	if (rp)
		memcpy(wp, rp, nframes * sizeof *wp);
	else
		memset(wp, 0,  nframes * sizeof *wp);
	ud->len += nframes;
	return status;
}

void initialize(void) {
	PaError err = Pa_Initialize();
	if (err != paNoError) {
		fprintf(stderr, "PortAudio initialization error: %s\n",
		  Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}
}

void terminate() {
	PaError err = Pa_Terminate();
	if (err != paNoError) {
		fprintf(stderr, "PortAudio termination error: %s\n",
		  Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}
}

void start(PaStream *stream) {
	PaError err = Pa_StartStream(stream);
	if (err != paNoError) {
		fprintf(stderr, "PortAudio start stream error: %s\n",
		  Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}
}

void stop(PaStream *stream) {
	PaError err = Pa_StopStream(stream);
	if (err != paNoError) {
		fprintf(stderr, "PortAudio start stream error: %s\n",
		  Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}
}

void idle(int seconds) {
	Pa_Sleep(seconds*1000);
}


int main() {
	PaStream *istream, *ostream;
	PaError err;

	initialize();

	struct recording ud = {
		.len = 0,
		.cap = NUM_SECONDS * SAMPLE_RATE,
		.data = calloc(ud.cap, sizeof *ud.data),
	};

	err = Pa_OpenDefaultStream(
	  &istream, 1, 0, paFloat32, SAMPLE_RATE, paFramesPerBufferUnspecified,
	  icallback, &ud);
	if (err != paNoError) {
		fprintf(stderr, "PortAudio open default stream error: %s\n",
		  Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}

	start(istream);
	while (Pa_IsStreamActive(istream)) {
		Pa_Sleep(100);
		printf("ud.len = %d\n", ud.len);
	}
	stop(istream);

	err = Pa_CloseStream(istream);
	if (err != paNoError) {
		fprintf(stderr, "PortAudio close stream error: %s\n",
		  Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}

	terminate();
}
