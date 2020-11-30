#define SAMPLE_RATE 44100
#define NUM_SECONDS 2

struct userdata {
	int len;
	int cap;
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
	struct userdata *ud = userdata;
	int status = paContinue;
	unsigned long rem = ud->cap - ud->len;
	const float *rp = input;
	float *wp = &ud->data[ud->len];

	(void) output;
	(void) timeinfo;
	(void) flags;

	if (rem < nframes) {
		nframes = rem;
		status = paComplete;
	}

	if (rp)
		memcpy(wp, rp, nframes * sizeof *wp);
	else
		memset(wp, 0,  nframes * sizeof *wp);

	ud->len += nframes;
	return status;
}

static int ocallback(
  const void *input,
  void *output,
  unsigned long nframes,
  const PaStreamCallbackTimeInfo* timeinfo,
  PaStreamCallbackFlags flags,
  void *userdata)
{
	struct userdata *ud = userdata;
	float *rp = &ud->data[ud->len];
	float *wp = output;
	unsigned int i;
	int status = paContinue;
	unsigned int rem = ud->cap - ud->len;

	(void) input;
	(void) timeinfo;
	(void) flags;

	if (rem < nframes) {
		memcpy(wp, rp, rem * sizeof *wp);
		memset(wp+i, 0, nframes-i);
		ud->len += rem;
		status = paComplete;
	} else {
		memcpy(wp, rp, nframes * sizeof *wp);
		ud->len += nframes;
	}
	return status;
}

int main(void) {
	PaStream*           stream;
	PaError             err = paNoError;
	struct userdata     ud;

	ud.cap = NUM_SECONDS * SAMPLE_RATE;
	ud.len = 0;
	ud.data = calloc(ud.cap, sizeof *ud.data);

	err = Pa_Initialize();
	if (err != paNoError)
		goto done;

	PaStreamParameters iparams = {
		.device = Pa_GetDefaultInputDevice(),
		.channelCount = 1,
		.sampleFormat = paFloat32,
		.suggestedLatency = 
		  Pa_GetDeviceInfo(iparams.device)->defaultLowInputLatency,
	};
	if (iparams.device == paNoDevice) {
		fprintf(stderr,"Error: No default input device.\n");
		goto done;
	}

	err = Pa_OpenStream(
	  &stream, &iparams, NULL, SAMPLE_RATE, paFramesPerBufferUnspecified,
	  paClipOff, icallback, &ud);
	if (err != paNoError)
		goto done;

	err = Pa_StartStream(stream);
	if (err != paNoError)
		goto done;

	puts("Recording...");
	fflush(stdout);

	while ( (err = Pa_IsStreamActive(stream)) == 1 ) {
		Pa_Sleep(1000);
		printf("index = %d\n", ud.len );
		fflush(stdout);
	}
	if (err < 0)
		goto done;

	err = Pa_CloseStream(stream);
	if (err != paNoError)
		goto done;

	ud.len = 0;

	PaStreamParameters oparams = {
		.device = Pa_GetDefaultOutputDevice(),
		.channelCount = 1,
		.sampleFormat = paFloat32,
		.suggestedLatency =
		  Pa_GetDeviceInfo(oparams.device)->defaultLowOutputLatency,
	};

	if (oparams.device == paNoDevice) {
		fprintf(stderr,"Error: No default output device.\n");
		goto done;
	}

	puts("Playing...");
	fflush(stdout);

	err = Pa_OpenStream(
	  &stream, NULL, &oparams, SAMPLE_RATE, paFramesPerBufferUnspecified,
	  paClipOff, ocallback, &ud);
	if (err != paNoError)
		goto done;

	if (stream) {
		err = Pa_StartStream(stream);
		if (err != paNoError)
			goto done;

		while ( (err = Pa_IsStreamActive(stream)) == 1 )
			Pa_Sleep(100);

		if (err < 0)
			goto done;

		err = Pa_CloseStream(stream);
		if (err != paNoError)
			goto done;
	}

done:
	Pa_Terminate();
	free(ud.data);
	if (err != paNoError) {
		fprintf(stderr, "error: %s\n", Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}
}
