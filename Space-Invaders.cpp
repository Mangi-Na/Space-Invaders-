#include <iostream>
#include <conio2.h>
#include <ctime>

using namespace std;

const int bordeSup = 1;
const int bordeIzq = 1;
const int bordeDer = 95;
const int bordeInf = 25;
const int MAX_BALAS = 5;
const int MAX_ENEMIGOS = 10;

// CLASE BASE 
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

// CLASE DERIVADA: ENEMIGO
class Enemigo : public Entidad {
public:
	Enemigo(int _x, int _y) : Entidad(_x, _y, YELLOW) {}
	
	void dibujar() override {
		if (!activo) return;
		textcolor(color);
		gotoxy(x, y);
		cout << 'O'; // Dibujo de alienígena básico
	}
	
	void moverPosicion(int dx, int dy) {
		if (!activo) return;
		borrar();
		x += dx;
		y += dy;
		dibujar();
	}
};

// CLASE DERIVADA: PROYECTIL
class Proyectil : public Entidad {
private:
	clock_t tempo;
	clock_t paso;
	
public:
	Proyectil(int _x, int _y) : Entidad(_x, _y, LIGHTCYAN) {
		paso = CLOCKS_PER_SEC / 40; // VELOCIDAD DEL DISPARO
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
			borrar();  // Borra donde estaba
			x--;       // Camina un paso a la izquierda
			dibujar(); // Se dibuja en la nueva posición
		}
	}
	
	void moverDerecha() {
		if (x < bordeDer - 1) {
			borrar();
			x++;       // Camina un paso a la derecha
			dibujar();
		}
	}
};

int main() {
	_setcursortype(_NOCURSOR); // Oculta el cursor de la consola
	
	// 1. Inicializar entidades
	Jugador nave(40, 22);
	nave.dibujar();
	
	Proyectil* balas[MAX_BALAS] = { NULL };
	
	Enemigo* enemigos[MAX_ENEMIGOS];
	for (int i = 0; i < MAX_ENEMIGOS; i++) {
		enemigos[i] = new Enemigo(10 + i * 5, 4); // Distribuidos en fila
		enemigos[i]->dibujar();
	}
	
	// Variables de control de enemigos
	int direccion = 1; // 1 = Derecha, -1 = Izquierda
	clock_t tempoEnemigos = clock();
	clock_t pasoEnemigos = CLOCKS_PER_SEC / 2; // Se mueven cada medio segundo
	
	// 2. BUCLE ÚNICO DEL JUEGO
	bool jugando = true;
	while (jugando) {
		
		// --- A. DETECCIÓN DE TECLAS (JUGADOR) ---
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
			if (tecla == 27) jugando = false; // Tecla ESC para salir
		}
		
		// --- B. ACTUALIZACIÓN DE PROYECTILES ---
		for (int i = 0; i < MAX_BALAS; i++) {
			if (balas[i] != NULL) {
				balas[i]->mover();
			}
		}
		
		// --- C. MOVIMIENTO AUTOMÁTICO DE ENEMIGOS EN BLOQUE ---
		if (clock() >= tempoEnemigos + pasoEnemigos) {
			bool cambiarDireccion = false;
			
			// 1. ¿Alguno tocó la pared lateral?
			for (int i = 0; i < MAX_ENEMIGOS; i++) {
				if (enemigos[i]->isActivo()) {
					if ((enemigos[i]->getX() >= bordeDer - 2 && direccion == 1) ||
						(enemigos[i]->getX() <= bordeIzq + 2 && direccion == -1)) {
						cambiarDireccion = true;
						break;
					}
				}
			}
			
			// 2. Si tocó la pared bajan 1 fila, si no avanzan a los costados
			int dx = cambiarDireccion ? 0 : direccion;
			int dy = cambiarDireccion ? 1 : 0;
			if (cambiarDireccion) direccion *= -1; // Invierte el sentido
			
			for (int i = 0; i < MAX_ENEMIGOS; i++) {
				enemigos[i]->moverPosicion(dx, dy);
			}
			
			tempoEnemigos = clock();
		}
	}
	
	// 3. LIBERACIÓN DE MEMORIA AL SALIR
	for (int i = 0; i < MAX_BALAS; i++) {
		delete balas[i];
	}
	for (int i = 0; i < MAX_ENEMIGOS; i++) {
		delete enemigos[i];
	}
	
	return 0;
}
