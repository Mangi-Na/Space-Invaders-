#include <iostream>
#include <conio2.h>
#include <ctime>

using namespace std;

const int bordeSup = 1;
const int bordeIzq = 1;
const int bordeDer = 119;
const int bordeInf = 25;
const int MAX_BALAS = 5;

//CLASE BASE 
class Entidad {
protected:
	int x, y;
	int color;
	bool activo;
	
public:
	Entidad(int _x, int _y, int _color) {
		x = _x;
		y = _y;
		color = _color;
		activo = true;
	}
	
		
	virtual void borrar() {
		gotoxy(x, y);
		cout << ' ';
	}
	
	virtual ~Entidad() {} // Destructor
	
	virtual void dibujar() = 0; // Cada objeto elegirá cómo dibujarse
	
	int getX() const { return x; }
	int getY() const { return y; }
	bool isActivo() const { return activo; }
};

//CLASE DERIVADA: PROYECTIL
class Proyectil : public Entidad {
private:
	clock_t tempo;
	clock_t paso;
	
public:
	Proyectil(int _x, int _y) : Entidad(_x, _y, LIGHTCYAN) {
		paso = CLOCKS_PER_SEC / 40; //VELOCIDAD DEL DISPARO
		tempo = clock();
	}
	
	void dibujar() override {
		if (!activo) return;
		textcolor(color);
		gotoxy(x, y);
		cout << '|';
	}
	
	void mover() {
		if (!activo) return;
		if (clock() >= tempo + paso) {
			borrar();
			y--;
			if (y <= bordeSup) {
				activo = false;
			} else {
				dibujar();
			}
			tempo = clock();
		}
	}
};

// NAVE (Jugador)
class Jugador : public Entidad {
public:
	Jugador(int _x, int _y) : Entidad(_x, _y, LIGHTGREEN) {}
	
	void dibujar() override {
		textcolor(color); 
		gotoxy(x, y);
		cout << "Z"; 
	}
	
	void moverIzquierda() {
		if (x > bordeIzq + 1) {
			borrar(); // Borra donde estaba
			x--;      // Camina un paso a la izquierda
			dibujar(); // Se dibuja en la nueva posición
		}
	}
	
	void moverDerecha() {
		if (x < bordeDer - 1) {
			borrar();
			x++;      // Camina un paso a la derecha
			dibujar();
		}
	}
};

int main() {
	
	Jugador nave(40, 22);
	nave.dibujar();
	
	Proyectil* balas[MAX_BALAS] = { NULL };
	
	bool jugando = true;
	while (jugando) {
		// Detección de teclas
		if (kbhit()) {
			char tecla = getch();
			if (tecla == 'a' || tecla == 'A') nave.moverIzquierda();
			if (tecla == 'd' || tecla == 'D') nave.moverDerecha();
			
			// Disparo
			if (tecla == ' ') {
				for (int i = 0; i < MAX_BALAS; i++) {
					if (balas[i] == NULL || !balas[i]->isActivo()) {
						delete balas[i];
						balas[i] = new Proyectil(nave.getX(), nave.getY() - 1);
						break; 
					}
				}
			}
			if (tecla == 27) jugando = false; // Tecla ESC
		}
		
		// Actualización de proyectiles
		for (int i = 0; i < MAX_BALAS; i++) {
			if (balas[i] != NULL) {
				balas[i]->mover();
			}
		}
	}
	
	// Liberación de memoria
	for (int i = 0; i < MAX_BALAS; i++) {
		delete balas[i];
	}
	
	return 0;
}
