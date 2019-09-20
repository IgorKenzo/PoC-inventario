#include <stdio.h>
// Inclui o arquivo de cabeçalho da biblioteca Allegro 5
#include <allegro5/allegro.h>
// Inclui o cabeçalho do add-on para uso de imagens
#include <allegro5/allegro_image.h>
// Inclui a coisa do mouse
#include <allegro5/allegro_native_dialog.h>

// Tamanho da tela
#define LARGURA_TELA 640
#define ALTURA_TELA 480

typedef struct Objeto
{
	ALLEGRO_BITMAP* bitmap;
	int x;
	int y;
	int largura;
	int altura;
	int cliqueX;
	int cliqueY;
} Objeto;

int main(void) {
	Objeto invetario[4][2];


	ALLEGRO_DISPLAY* janela = NULL;

	Objeto imagem;
	// Variável para imagem
	imagem.bitmap = NULL;
	imagem.x = 0;
	imagem.y = 0;
	imagem.largura = 100;
	imagem.altura = 100;

	Objeto retangulo;
	retangulo.bitmap = NULL;
	retangulo.x = LARGURA_TELA - 150;
	retangulo.y = ALTURA_TELA - 150;
	retangulo.altura = 150;
	retangulo.largura = 150;

	Objeto certo;
	certo.bitmap = NULL;
	certo.x = LARGURA_TELA - 100;
	certo.y = 0;
	certo.largura = 100;
	certo.altura = 100;

	//declara a fila de eventos
	ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;

	if (!al_init()) {
		fprintf(stderr, "Falha ao iniciar Allegro\n");
		return -1;
	}
	// Inicializa o add-on para utilização de imagens
	al_init_image_addon();

	// Criamos a nossa janela - dimensões de 640x480 px
	janela = al_create_display(LARGURA_TELA, ALTURA_TELA);
	if (!janela) {
		fprintf(stderr, "Falha ao iniciar janela\n");
		return -1;
	}
	// Configura o título da janela
	al_set_window_title(janela, "Arrastar coisas");

	imagem.bitmap = al_load_bitmap("Imgs/photo.bmp");
	retangulo.bitmap = al_load_bitmap("Imgs/ret.bmp");
	certo.bitmap = al_load_bitmap("Imgs/ok.bmp");
	if (!imagem.bitmap || !retangulo.bitmap || !certo.bitmap) {
		fprintf(stderr, "Falha ao iniciar imagem\n");
		al_destroy_display(janela);
		return -1;
	}

	fila_eventos = al_create_event_queue();
	if (!fila_eventos) {
		fprintf(stderr, "Falha ao criar fila de evento\n");
		al_destroy_display(janela);
		al_destroy_bitmap(imagem.bitmap);
		al_destroy_bitmap(retangulo.bitmap);
		al_destroy_bitmap(certo.bitmap);
		return -1;
	}

	// Torna apto o uso de mouse na aplicação
	if (!al_install_mouse()) {
		fprintf(stderr, "Falha ao iniciar o mouse\n");
		al_destroy_display(janela);
		al_destroy_bitmap(imagem.bitmap);
		al_destroy_bitmap(retangulo.bitmap);
		al_destroy_bitmap(certo.bitmap);
		return -1;
	}
	// Atribui o cursor padrão do sistema para ser usado
	if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT)) {
		fprintf(stderr, "Falha ao atribur ponteiro ao mouse\n");
		al_destroy_display(janela);
		al_destroy_bitmap(imagem.bitmap);
		al_destroy_bitmap(retangulo.bitmap);
		al_destroy_bitmap(certo.bitmap);
		return -1;
	}

	//registra eventos da janela em fila_eventos
	al_register_event_source(fila_eventos, al_get_display_event_source(janela));
	//registra eventos da janela em fila_eventos
	al_register_event_source(fila_eventos, al_get_mouse_event_source());

	////////////////////////////////////////////////

	// Preenchemos a janela de branco
	al_clear_to_color(al_map_rgb(255, 255, 255));

	//desenha a imagem na tela
	al_draw_bitmap(retangulo.bitmap, retangulo.x, retangulo.y, 0);
	al_draw_bitmap(imagem.bitmap, imagem.x, imagem.y, 0);

	// Atualiza a tela
	al_flip_display();

	int gameOver = 0;
	int Arrastando = 0;
	int correto = 0;
	while (!gameOver) {
		while (!al_is_event_queue_empty(fila_eventos))
		{
			//Cria um evento
			ALLEGRO_EVENT evento;
			//espero por um evento da fila, e guarda em evento
			al_wait_for_event(fila_eventos, &evento);

			//se teve eventos e foi um evento de fechar janela, encerra repetição			
			if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
				gameOver = 1;
			}
			else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
				if (IsInside(evento.mouse.x, evento.mouse.y, imagem) && !Arrastando) {
					Arrastando = 1;
					imagem.cliqueX = MapearDistancia(evento.mouse.x, imagem.x);
					imagem.cliqueY = MapearDistancia(evento.mouse.y, imagem.y);
				}
				else
				{
					Arrastando = 0;
				}
			}
			else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
				Arrastando = 0;
			}

			ALLEGRO_MOUSE_STATE state;


			al_get_mouse_state(&state);
			if (state.buttons & 1 && Arrastando) {
				/* Primary (e.g. left) mouse button is held. */
				if (!VerificarBordas(evento.mouse.x, evento.mouse.y, imagem)) {
					imagem.x = evento.mouse.x - imagem.cliqueX;
					imagem.y = evento.mouse.y - imagem.cliqueY;
				}
			}
			if (Arrastando && IsInsideImagem(imagem, retangulo)) {
				correto = 1;
			}
			else if (Arrastando && !IsInsideImagem(imagem, retangulo)) {
				correto = 0;
			}
		}

		al_clear_to_color(al_map_rgb(255, 255, 255));

		al_draw_bitmap(retangulo.bitmap, retangulo.x, retangulo.y, 0);
		al_draw_bitmap(imagem.bitmap, imagem.x, imagem.y, 0);
		if (correto) {
			al_draw_bitmap(certo.bitmap, certo.x, certo.y, 0);
		}

		al_flip_display();
	}
	// Finaliza a janela
	al_destroy_display(janela);

	// Finaliza imagem
	al_destroy_bitmap(imagem.bitmap);
	al_destroy_bitmap(retangulo.bitmap);
	al_destroy_bitmap(certo.bitmap);

	// Finaliza fila de eventos
	al_destroy_event_queue(fila_eventos);

	return 0;
}

int IsInside(int x, int y, Objeto objeto) {
	if (x >= objeto.x && x <= (objeto.x + objeto.largura) && y >= objeto.y && y <= (objeto.y + objeto.altura)) {
		return 1;
	}
	return 0;
}

int MapearDistancia(int ponto, int x) {
	return ponto - x;
}

int VerificarBordas(int x, int y, Objeto imagem) {
	if (x - imagem.cliqueX <= 0 || x - imagem.cliqueX + imagem.largura >= LARGURA_TELA || y - imagem.cliqueY <= 0 || y - imagem.cliqueY + imagem.altura >= ALTURA_TELA) {
		return 1;
	}
	return 0;
}

int IsInsideImagem(Objeto menor, Objeto maior) {
	if (menor.x >= maior.x && menor.x + menor.largura <= maior.x + maior.largura && menor.y >= maior.y && menor.y + menor.altura <= maior.y + maior.altura) {
		return 1;
	}
	return 0;
}