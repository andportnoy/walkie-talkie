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
	unsigned int framesLeft = ud->cap - ud->len;

	(void) input;
	(void) timeinfo;
	(void) flags;

	if (framesLeft < nframes) {
		memcpy(wp, rp, framesLeft * sizeof *wp);
		memset(wp+i, 0, nframes-i);
		ud->len += framesLeft;
		status = paComplete;
	} else {
		memcpy(wp, rp, nframes * sizeof *wp);
		ud->len += nframes;
	}
	return status;
}

int main(void) {
	PaStreamParameters  inputParameters,
			    outputParameters;
	PaStream*           stream;
	PaError             err = paNoError;
	struct userdata     ud;

	ud.cap = NUM_SECONDS * SAMPLE_RATE; /* Record for a few seconds. */
	ud.len = 0;
	ud.data = calloc(ud.cap, sizeof *ud.data);

	err = Pa_Initialize();
	if (err != paNoError)
		goto done;

	inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
	if (inputParameters.device == paNoDevice) {
		fprintf(stderr,"Error: No default input device.\n");
		goto done;
	}
	inputParameters.channelCount = 1;
	inputParameters.sampleFormat = paFloat32;
	inputParameters.suggestedLatency =
		Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	/* Record some audio. -------------------------------------------- */
	err = Pa_OpenStream(
			&stream,
			&inputParameters,
			NULL,                  /* &outputParameters, */
			SAMPLE_RATE,
			paFramesPerBufferUnspecified,
			paClipOff,      /* we won't output out of range samples so don't bother clipping them */
			icallback,
			&ud );
	if( err != paNoError ) goto done;

	err = Pa_StartStream( stream );
	if( err != paNoError ) goto done;
	printf("\n=== Now recording!! Please speak into the microphone. ===\n"); fflush(stdout);

	while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
	{
		Pa_Sleep(1000);
		printf("index = %d\n", ud.len ); fflush(stdout);
	}
	if( err < 0 ) goto done;

	err = Pa_CloseStream( stream );
	if( err != paNoError ) goto done;

	/* Playback recorded data.  -------------------------------------------- */
	ud.len = 0;

	outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
	if (outputParameters.device == paNoDevice) {
		fprintf(stderr,"Error: No default output device.\n");
		goto done;
	}
	outputParameters.channelCount = 1;                     /* stereo output */
	outputParameters.sampleFormat =  paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	printf("\n=== Now playing back. ===\n"); fflush(stdout);
	err = Pa_OpenStream(
			&stream,
			NULL, /* no input */
			&outputParameters,
			SAMPLE_RATE,
			paFramesPerBufferUnspecified,
			paClipOff,      /* we won't output out of range samples so don't bother clipping them */
			ocallback,
			&ud );
	if( err != paNoError ) goto done;

	if( stream )
	{
		err = Pa_StartStream( stream );
		if( err != paNoError ) goto done;

		printf("Waiting for playback to finish.\n"); fflush(stdout);

		while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
		if( err < 0 ) goto done;

		err = Pa_CloseStream( stream );
		if( err != paNoError ) goto done;

		printf("Done.\n"); fflush(stdout);
	}

done:
	Pa_Terminate();
	free(ud.data);
	if (err != paNoError) {
		fprintf(stderr, "error: %s\n", Pa_GetErrorText(err));
		exit(EXIT_FAILURE);
	}
}
