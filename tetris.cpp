#include <allegro.h>
#include <time.h>

// Rozdzielczosc
#define screenX 640
#define screenY 480

// Przesuniecie planszy wzgledem lewej strony ekranu
#define dx 220

// Wielkosc planszy
#define sX 10
#define sY 24

// 1000 / fps
#define RestTime 50

// Poczatkowa predkosc (co ile klatek spada blok?)
#define StartSpeed 10

// Przyspieszenie o 1 co taka liczbe linijek
#define Accel 10;

// Macro - losowy kolor
#define randCol (((rand() % 62) + 1) * 3)
// Macro
#define candCol(c) ((candy == 0)? c: randCol);

// Buffer
BITMAP *bmp;

// Paleta
PALETTE pal;

// Fullscreen?
int fullscreen = 0;
int candy = 0;

// Muzyka
DATAFILE *mus;

// Tlo
RLE_SPRITE *back;

/*


	Kody tetramina


*/

typedef int Tetramino;

#define	I 0
#define	T 1
#define	O 2
#define	L 3
#define	J 4
#define	S 5
#define Z 6



/*


	Struktura: Block


*/

struct Block
{
	int Matrix[4][4];

	// Konstruktor & destruktor
	Block(Tetramino t, int col)
	{
		clrMatrix();
		createMatrix(t, col);
	}

	~Block() {}

	// Czyszczenie macierzy
	void clrMatrix() {
		for (int x = 0; x < 4; x++)
	    	for (int y = 0; y < 4; y++)
    	    	Matrix[x][y] = 0;
	}

	// Obracanie macierzy o 90 stopni w lewo
	void rotateMatrix()
	{
		int newMatrix[4][4];
		for (int x = 0; x < 4; x++)
		    for (int y = 0; y < 4; y++)
				newMatrix[x][y] = Matrix[3 - y][x];
		for (int x = 0; x < 4; x++)
	    	for (int y = 0; y < 4; y++)
				Matrix[x][y] = newMatrix[x][y];
	}

	// Tworzenie macierzy wg danego tetramino
	void createMatrix(Tetramino t, int v)
	{
		switch (t)
		{
			case I:
				Matrix[1][0] = candCol(v); Matrix[1][1] = candCol(v); Matrix[1][2] = candCol(v); Matrix[1][3] = candCol(v);
				break;
			case T:
    	        Matrix[0][1] = candCol(v); Matrix[1][1] = candCol(v); Matrix[2][1] = candCol(v); Matrix[1][2] = candCol(v);
        	    break;
			case O:
    	        Matrix[1][1] = candCol(v); Matrix[1][2] = candCol(v); Matrix[2][1] = candCol(v); Matrix[2][2] = candCol(v);
        	    break;
			case L:
    	        Matrix[1][0] = candCol(v); Matrix[1][1] = candCol(v); Matrix[1][2] = candCol(v); Matrix[2][2] = candCol(v);
        	    break;
			case J:
    	        Matrix[2][0] = candCol(v); Matrix[2][1] = candCol(v); Matrix[2][2] = candCol(v); Matrix[1][2] = candCol(v);
        	    break;
			case S:
    	        Matrix[1][1] = candCol(v); Matrix[1][2] = candCol(v); Matrix[2][1] = candCol(v); Matrix[0][2] = candCol(v);
        	    break;
			case Z:
    	        Matrix[1][1] = candCol(v); Matrix[1][2] = candCol(v); Matrix[0][1] = candCol(v); Matrix[2][2] = candCol(v);
        	    break;
		}
		
	}
	
	// Rysowanie klocka
	void Show(int ax, int ay)
	{
		for (int x = 0; x < 4; x++)
		    for (int y = 0; y < 4; y++)
	        	{
					if (Matrix[x][y] == 0) continue;
    	            rectfill(bmp, dx + ax + x * 20 + 1, ay + y * 20 + 1, dx + ax + x * 20 + 18, ay + y * 20 + 18, Matrix[x][y]);
	                vline(bmp, dx + ax + x * 20, ay + y * 20, ay + y * 20 + 19, Matrix[x][y] + 1);
                	vline(bmp, dx + ax + x * 20 + 19, ay + y * 20 + 1, ay + y * 20 + 19, Matrix[x][y] + 2);
					hline(bmp, dx + ax + x * 20 + 1, ay + y * 20, dx + ax + x * 20 + 19, Matrix[x][y] + 1);
					hline(bmp, dx + ax + x * 20 + 1, ay + y * 20 + 19, dx + ax + x * 20 + 18, Matrix[x][y] + 2);
				}
	}
	
	// sprawdzanie czy klocek moze znalezc sie w otoczeniu newMatrix
	bool isOK(int newMatrix[4][4])
	{
	    for (int x = 0; x < 4; x++)
		    for (int y = 0; y < 4; y++)
		        if ((newMatrix[x][y] != 0) && (Matrix[x][y] != 0))
	    	    {
					return false;
				}
		return true;
	}
};

/*


	Struktura: plansza


*/

struct Board
{
	int Matrix[sX][sY];
	
	// Konstruktor i destruktor
	Board()
	{
		for (int x = 0; x < sX; x++)
	    	for (int y = 0; y < sY; y++)
	        	Matrix[x][y] = 0;
	}
	
	~Board()
	{
		int c = randCol;
		for (int y = sY - 1; y >= 0; y--)
		{
		    for (int x = 0; x < sX; x++) Matrix[x][y] = candCol(c);
        	Show(); blit(bmp, screen, 0, 0, 0, 0, 640, 480); rest(50);
		}
		rest(150);
		for (int y = 0; y < sY; y++)
		{
		    for (int x = 0; x < sX; x++) Matrix[x][y] = 0;
		    draw_rle_sprite(bmp, back, 0, 0);
	        Show(); blit(bmp, screen, 0, 0, 0, 0, 640, 480); rest(50);
		}
	}
	
	// Wstawianie klocka
	void Wstaw(int px, int py, Block k)
	{
		for (int x = 0; x < 4; x++)
	    	for (int y = 0; y < 4; y++)
				if (k.Matrix[x][y] > 0) Matrix[px + x][py + y] = k.Matrix[x][y];
	}
	
	// Sprawdzanie, czy dany klocek mozna wstawic w dane miejsce
	bool isOK(int px, int py, Block k)
	{
    	int Sub[4][4];
		for (int x = 0; x < 4; x++)
		    for (int y = 0; y < 4; y++)
		        if ((x + px >= 0) && (x + px < sX) &&
	            	(y + py >= 0) && (y + py < sY))
						Sub[x][y] = Matrix[x+px][y+py];
				else    Sub[x][y] = 3;
		return k.isOK(Sub);
	}
	
	// Rysuje plansze
	void Show()
	{
	for (int x = 0; x < sX; x++)
	    for (int y = 0; y < sY; y++)
	        {
				if (Matrix[x][y] == 0)
				{
                   // rectfill(bmp, dx + x * 20, y * 20, dx + x * 20 + 19, y * 20 + 19, 0);
				} else {
                rectfill(bmp, dx + x * 20 + 1, y * 20 + 1, dx + x * 20 + 18, y * 20 + 18, Matrix[x][y]);
                vline(bmp, dx + x * 20, y * 20, y * 20 + 19, Matrix[x][y] + 1);
                vline(bmp, dx + x * 20 + 19, y * 20 + 1, y * 20 + 19, Matrix[x][y] + 2);
				hline(bmp, dx + x * 20 + 1, y * 20, dx + x * 20 + 19, Matrix[x][y] + 1);
				hline(bmp, dx + x * 20 + 1, y * 20 + 19, dx + x * 20 + 18, Matrix[x][y] + 2);
				}
			}
	}

	// Sprawdza, czy sa pelne rzedy i usuwa je
	int ClearFull()
	{
		int l = 0;

		for (int y = sY; y >= 0; y--)
		{
			bool clr = true;
			for (int x = 0; x < sX; x++)
				clr = clr && (Matrix[x][y] != 0);
			if (clr)
			{
				l++;
				for (int i = 0; i < 10; i++) {
				for (int x = 0; x < sX; x++) Matrix[x][y] = 0; draw_rle_sprite(bmp, back, 0, 0); Show(); blit(bmp, screen, 0, 0, 0, 0, 640, 480); rest(10);
    	        for (int x = 0; x < sX; x++) Matrix[x][y] = randCol; draw_rle_sprite(bmp, back, 0, 0); Show(); blit(bmp, screen, 0, 0, 0, 0, 640, 480); rest(10);
			}
				for (int i = y; i > 0; i--)
				for (int x = 0; x < sX; x++)
					Matrix[x][i] = Matrix[x][i - 1];
				y = sY;
			}
		}
		return l;
	}
};

/*


	* * * G R A * * *


*/

int game()
{
	// Pozycja bloku
	int x = (sX / 2) - 2, y = 0;
	// Timing 
	int t = 0;
	// Punkty i linie
	int pkt = 0, lns = 0;
	// Predkosc i przyspieszenie
	int accel = Accel;
	int speed = StartSpeed;
	
	// Zmienna - konieczna aby przy 1 nacisnieciu spacji nastepowal 1 obrot klocka
	bool rotated = false;

	// Tu sie toczy gra
	Board plansza;
	
	// Aktualny i nastepny blok
	Block *next = new Block(rand() % 7, randCol);
	Block *kloc = new Block(rand() % 7, randCol);

	// Glowna petla
	while (!key[KEY_ESC]) {
		// Rysowanie tla
		draw_rle_sprite(bmp, back, 0, 0);
		
		// Rysowanie planszy
        plansza.Show();
        
        // Rysowanie spadajacego bloku
		kloc->Show(x*20,y*20);
		
		// Timing
		t++;

		// Przesuwanie bloku
		if (key[KEY_LEFT]) { if (plansza.isOK(x - 1, y, *kloc)) x--; }
		if (key[KEY_RIGHT]) { if (plansza.isOK(x + 1, y, *kloc)) x++; }

		// Obracanie bloku
		if (key[KEY_SPACE]) {
            if (!rotated) {
				do {
				kloc->rotateMatrix();
				} while (!plansza.isOK(x, y, *kloc));
				rotated = true;
			 }
			} else rotated = false;

		// Spadanie
		if (key[KEY_DOWN] || (t >= speed))
		{
			t = 0;

			// Blok nie moze dalej opadac?
			if (plansza.isOK(x, y + 1, *kloc)) y++;
			else
			{
				// Wstaw blok
				plansza.Wstaw(x, y, *kloc);

				// Wyczyszczenie linijek pelnych
				int l = plansza.ClearFull();

				// Zwiekszamy predkosc?
				accel -= l;
				if (accel <= 0)
				{
					accel += Accel;
					if (speed > 1) speed--;
				}

				// Dodanie punktow
				lns += l;
				while (l > 0) pkt += (l--);

				// Utworzenie nowego bloku
				delete kloc;
				x = (sX / 2) - 2, y = 0;
				kloc = next;
				next = NULL;
				next = new Block(rand() % 7, randCol);

				// GAME OVER?
				if (!plansza.isOK(x, y, *kloc)) break;
			}
		}
		next->Show(230, 50);
		
		// Punkty
        textprintf_right_ex(bmp, font, 120, 30, 0, -1, "%d00", pkt);
        // Linie
        textprintf_right_ex(bmp, font, 120, 85, 0, -1, "%d", lns);
        // Predkosc
        textprintf_right_ex(bmp, font, 120, 140, 0, -1, "%d", StartSpeed + 1 - speed);
        // Wrzucenie na ekran
		blit(bmp, screen, 0, 0, 0, 0, screenX, screenY);
        // Timing
		rest(50);
	}

	delete kloc;
	delete next;

	return pkt;
}

/*


	Nudne procedury - Main, Init, Deinit, Winful...


*/

void winfull();
void init();
void deinit();

/*


	* * * MAIN * * *


*/

int main() {

    int pts = 0, top = 0;
	int quit = 0;

	init();
	
	play_midi((MIDI*)(mus->dat), 255);

	draw_rle_sprite(bmp, back, 0, 0);
	textprintf_centre_ex(bmp, font, 320, 160, 0, -1, "Michal Lesniewski");
	textprintf_centre_ex(bmp, font, 320, 175, 0, -1, "* T E T R I S *");

	textprintf_centre_ex(bmp, font, 320, 200, 0, -1, "Press SPACE to play again or ESC to quit");
	textprintf_centre_ex(bmp, font, 320, 430, 0, -1, "ALT + ENTER switches WINDOW/FULLSCREEN");
	if (!mus)
		textprintf_centre_ex(bmp, font, 320, 450, 0, -1, "datafile error!");

	while (quit != 1) {
		quit = 0;

		if (key[KEY_ESC]) quit = 1;
		if (key[KEY_C]) candy = 1;
		if (key[KEY_SPACE])
		{
            pts = game();
            if (top < pts) top = pts;
			draw_rle_sprite(bmp, back, 0, 0);
			textprintf_centre_ex(bmp, font, 320, 160, 0, -1, "GAME OVER");
			textprintf_centre_ex(bmp, font, 320, 175, 0, -1, "Your last score: %d00", pts);
			textprintf_centre_ex(bmp, font, 320, 185, 0, -1, "Your best score: %d00", top);
			textprintf_centre_ex(bmp, font, 320, 200, 0, -1, "Press SPACE to play again or ESC to quit");
			textprintf_centre_ex(bmp, font, 320, 430, 0, -1, "ALT + ENTER switches WINDOW/FULLSCREEN");
		}
	
	    if ((key_shifts & KB_ALT_FLAG) && (key[KEY_ENTER])) winfull();

	    
		blit(bmp, screen, 0, 0, 0, 0, screenX, screenY);
		rest(RestTime);
	}

	deinit();
	return 0;
}
END_OF_MAIN()


// ----- Przelaczanie na pelny ekran -----
void winfull()
{
    fullscreen = ++fullscreen % 2;
	int res;
    if (fullscreen == 0)
		res = set_gfx_mode(GFX_AUTODETECT_WINDOWED, screenX, screenY, 0, 0);
	else
	    res = set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, screenX, screenY, 0, 0);
	if (res != 0) {
		allegro_message(allegro_error);
		deinit();
		exit(-1);
	}
	set_palette(pal);
	rest(500);
}


// ----- Init / Deinit -----
void init() {
	int res;
	allegro_init();
	set_color_depth(8);
	res = set_gfx_mode(GFX_AUTODETECT_WINDOWED, screenX, screenY, 0, 0);
	if (res != 0) {
		allegro_message(allegro_error);
		exit(-1);
	}
	install_timer();
	install_keyboard();
	bmp = create_bitmap(screenX, screenY);

	// Tworzenie palety
	int c = 0;
	RGB rgb;
	for (rgb.r = 0; rgb.r <= 63; rgb.r += 21)
		for (rgb.g = 0; rgb.g <= 63; rgb.g += 21)
			for (rgb.b = 0; rgb.b <= 63; rgb.b += 21)
			    {
					RGB l = rgb, s = rgb;
					if (l.r <= 47) l.r += 16;
                    if (l.g <= 47) l.g += 16;
                    if (l.b <= 47) l.b += 16;
                    if (s.r >= 16) s.r -= 16;
					if (s.g >= 16) s.g -= 16;
					if (s.b >= 16) s.b -= 16;
					pal[c++] = rgb;
					pal[c++] = l;
					pal[c++] = s;
				/*	set_color(c++, &rgb);
					set_color(c++, &l);
					set_color(c++, &s); */
				}

    set_palette(pal);
    
	install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, "");
	
	// Wczytanie muzyki
	mus = load_datafile_object("tetris.dat", "music");
	
	// Wczytanie tla
	set_color_conversion(COLORCONV_DITHER_PAL + COLORCONV_24_TO_8);
	DATAFILE *data = load_datafile_object("tetris.dat", "back");
	
	if (!data)
	    back = NULL;
	else
		back = get_rle_sprite((BITMAP*)(data->dat));
		
	unload_datafile_object(data);

	srand(time(NULL));
}

void deinit() {
	clear_keybuf();
	destroy_bitmap(bmp);
	destroy_rle_sprite(back);
	unload_datafile_object(mus);
}
