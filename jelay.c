#include <stdio.h>
#include <unistd.h>
#include <jack/jack.h>
#include <SDL/SDL.h>

jack_port_t* input_port1;

jack_port_t* output_port1;


float delay1 = 0;
float delay2 = 0;

float a1val = 0;
float a2val = 0;
float b1val = 0;
float b2val = 0;

/* Copie les entrées vers les sorties. */
int process(jack_nframes_t nframes, void*arg)
{
    /* Récupère les buffers liés au 4 ports. */
    float* in1 = jack_port_get_buffer(input_port1, nframes);

    float* out1 = jack_port_get_buffer(output_port1, nframes);

    /* Boucle de traitement sur les échantillons. */
    int i;
	float buffer = 0;
	
    for (i = 0; i < nframes; i = i + 1) {
        // Traitement sur les échantillons
        buffer = in1[i] + delay1*a1val + delay2*a2val;
		out1[i] = buffer + delay1*b1val + delay2*b2val;
		delay2 = delay1;
		delay1 = buffer;	
		
		}

    return 0;      
}

/*
Une application audio avec deux ports audio d'entrée et deux ports audio de sortie.
*/

int main()
{
	
if(SDL_Init(SDL_INIT_VIDEO)==-1)
	{
	fprintf(stderr,"erreur a l'initialisation de sdl : %s\n",SDL_GetError());
	exit(EXIT_FAILURE);
	}

SDL_Surface *ecran = NULL;
SDL_WM_SetCaption("biquad", NULL);
//SDL_WM_SetIcon(SDL_LoadBMP("files/sdl_icone.bmp"),NULL);
ecran = SDL_SetVideoMode(508,500,32,SDL_HWSURFACE);
SDL_FillRect(ecran,NULL,SDL_MapRGB(ecran->format,50,50,60));

SDL_Surface *fond = NULL;
fond = SDL_LoadBMP("files/fond.bmp");
SDL_Rect fondPos;
fondPos.x = 0;
fondPos.y = 0;

SDL_Surface *a1 = NULL;
a1 = SDL_LoadBMP("files/slider0.bmp");
SDL_Rect aPos1;
aPos1.x = 120 - a1->w / 2;
aPos1.y = 320 - a1->h/2;

SDL_Surface *a2 = NULL;
a2 = SDL_LoadBMP("files/slider1.bmp");
SDL_Rect aPos2;
aPos2.x = 120 - a2->w / 2;
aPos2.y = 368 - a2->h/2;

SDL_Surface *b1 = NULL;
b1 = SDL_LoadBMP("files/slider2.bmp");
SDL_Rect bPos1;
bPos1.x = 120 - b1->w / 2;
bPos1.y = 418 - b1->h/2;

SDL_Surface *b2 = NULL;
b2 = SDL_LoadBMP("files/slider3.bmp");
SDL_Rect bPos2;
bPos2.x = 120 - b2->w / 2;
bPos2.y = 466 - b2->h/2;

int continuer = 1;
SDL_Event event;

int sliderActif = 0;
float maxX = 450.0f;
int minX = 100;



    jack_options_t options = JackNullOption;
    jack_status_t status;

    /* Ouvrir le client JACK */
    jack_client_t* client = jack_client_open("Biquad", options, &status);

    /* Ouvrir les ports en entrée et en sortie */
    input_port1 = jack_port_register(client, "input1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    output_port1 = jack_port_register(client, "output1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    /* Enregister le traitement qui sera fait à chaque cycle */
    jack_set_process_callback(client, process, NULL);

    jack_activate(client);

    /* Fonctionne jusqu'à ce que Ctrl+C soit utilisé */
    printf("Utiliser 'echap' pour quitter l'application...\n");


while (continuer)
{
	SDL_WaitEvent(&event);
	switch(event.type)
	{
		case SDL_QUIT:
		continuer = 0;
		break;

		case SDL_KEYDOWN:
		switch(event.key.keysym.sym)
		{
			case SDLK_ESCAPE:
			continuer = 0;
			break;
		}
		break;

		case SDL_MOUSEBUTTONDOWN:
		if(event.button.y<345){sliderActif = 1;}
		else if(event.button.y<395){sliderActif = 2;}
		else if(event.button.y<445){sliderActif = 3;}
		else {sliderActif = 4;}
		break;

		case SDL_MOUSEBUTTONUP:
		sliderActif = 0;
		break;

		case SDL_MOUSEMOTION:
		switch(sliderActif)
		{
			case 1:
			aPos1.x = event.motion.x - a1->h/2;
			break;

			case 2:
			aPos2.x = event.motion.x - a2->h/2;
			break;

			case 3:
			bPos1.x = event.motion.x - b1->h/2;
			break;

			case 4:
			bPos2.x = event.motion.x - b2->h/2;
			break;


		}
		break;
	}
	if(aPos1.x > maxX){aPos1.x = maxX;}
	if(aPos1.x < minX){aPos1.x = minX;}
	if(aPos2.x > maxX){aPos2.x = maxX;}
	if(aPos2.x < minX){aPos2.x = minX;}
	if(bPos1.x > maxX){bPos1.x = maxX;}
	if(bPos1.x < minX){bPos1.x = minX;}
	if(bPos2.x > maxX){bPos2.x = maxX;}
	if(bPos2.x < minX){bPos2.x = minX;}
	
	a1val = ((aPos1.x-minX)/(maxX-minX))*2-1;
	a2val = ((aPos2.x-minX)/(maxX-minX))*2-1;
	b1val = ((bPos1.x-minX)/(maxX-minX))*2-1;
	b2val = ((bPos2.x-minX)/(maxX-minX))*2-1;
	
	SDL_FillRect(ecran,NULL,SDL_MapRGB(ecran->format,0,0,0));
	SDL_BlitSurface(fond,NULL,ecran,&fondPos);
	SDL_BlitSurface(a1,NULL,ecran,&aPos1);
	SDL_BlitSurface(a2,NULL,ecran,&aPos2);
	SDL_BlitSurface(b1,NULL,ecran,&bPos1);
	SDL_BlitSurface(b2,NULL,ecran,&bPos2);
	
	SDL_Flip(ecran);
}

SDL_FreeSurface(fond);
SDL_FreeSurface(a1);
SDL_FreeSurface(a2);
SDL_FreeSurface(b1);
SDL_FreeSurface(b2);
SDL_Quit();

    jack_deactivate(client);
    jack_client_close(client);
    return 0;
}
