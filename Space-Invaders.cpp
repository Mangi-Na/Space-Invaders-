#include <iostream>
#include <conio2.h>
#include <ctime>
#include <cstdlib>

using namespace std;

const int bordeSup = 1;
const int bordeIzq = 1;
const int bordeDer = 95;
const int bordeInf = 25;
const int MAX_BALAS = 5;
const int MAX_BALAS_ENEMIGAS = 5;
const int FILAS_ENEMIGOS = 3;
const int COLS_ENEMIGOS = 8;
const int MAX_ENEMIGOS = FILAS_ENEMIGOS * COLS_ENEMIGOS;

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
	void setActivo(bool _activo) { activo = _activo; }
};

// CLASE DERIVADA: ENEMIGO
class Enemigo : public Entidad {
private:
	char caracter; // 'M', 'W', 'v'
	int vida;      // Puntos de resistencia (1, 2, 3...)
	
public:
	Enemigo(int _x, int _y, int _color, char _caracter, int _vida) 
		: Entidad(_x, _y, _color), caracter(_caracter), vida(_vida) {}
	
	void dibujar() override {
		if (!activo) return;
		textcolor(color);
		gotoxy(x, y);
		cout << caracter; // Dibuja la forma específica del enemigo
	}
	
	void moverPosicion(int dx, int dy) {
		if (!activo) return;
		borrar();
		x += dx;
		y += dy;
		dibujar();
	}
	
	// MÉTODO AGREGADO: Resta vida y desactiva el enemigo cuando llega a 0
	bool recibirDano() {
		vida--;
		if (vida <= 0) {
			borrar();
			activo = false;
			return true; //Fue eliminado
		}
		return false; //Sigue vivo
	}
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
// NUEVA CLASE DERIVADA: PROYECTIL ENEMIGO (Va hacia abajo)
class ProyectilEnemigo : public Entidad {
private:
	clock_t tempo;
	clock_t paso;
	
public:
	ProyectilEnemigo(int _x, int _y) : Entidad(_x, _y, LIGHTRED) {
		paso = CLOCKS_PER_SEC / 20; // Velocidad del disparo enemigo
		tempo = clock();
	}
	
	void dibujar() override {
		if (!activo) return;
		textcolor(color);
		gotoxy(x, y);
		cout << '*';
	}
	
	void mover() {
		if (!activo) return;
		if (clock() >= tempo + paso) {
			borrar();
			y++;
			if (y >= bordeInf) {
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
	Jugador(int _x, int _y) : Entidad(_x, _y, YELLOW) {}
	
	void dibujar() override {
		textcolor(color); 
		gotoxy(x, y);
		cout << "X"; 
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

void verificarColisiones(Proyectil* balas[], Enemigo* enemigos[]) {
	for (int i = 0; i < MAX_BALAS; i++) {
		if (balas[i] != NULL && balas[i]->isActivo()) {
			for (int j = 0; j < MAX_ENEMIGOS; j++) {
				if (enemigos[j] != NULL && enemigos[j]->isActivo()) {
					if (balas[i]->getX() == enemigos[j]->getX() && 
						balas[i]->getY() == enemigos[j]->getY()) {
						
						// La bala siempre desaparece al chocar
						balas[i]->borrar();
						balas[i]->setActivo(false); 
						
						// El enemigo recibe daño y solo muere si su vida llega a 0
						enemigos[j]->recibirDano();
						
						break;
					}
				}
			}
		}
	}
}
// Detecta si una bala enemiga impacta en la nave del jugador
bool verificarColisionJugador(ProyectilEnemigo* balasEnemigas[], Jugador& nave) {
	for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++) {
		if (balasEnemigas[i] != NULL && balasEnemigas[i]->isActivo()) {
			if (balasEnemigas[i]->getX() == nave.getX() && 
				balasEnemigas[i]->getY() == nave.getY()) {
				return true; // Hubo colisión
			}
		}
	}
	return false;
}

// Selecciona un enemigo activo al azar para que dispare hacia abajo
void generarDisparoEnemigo(Enemigo* enemigos[], ProyectilEnemigo* balasEnemigas[]) {
	// Busca un espacio libre en el arreglo de balas enemigas
	int slotBala = -1;
	for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++) {
		if (balasEnemigas[i] == NULL || !balasEnemigas[i]->isActivo()) {
			slotBala = i;
			break;
		}
	}
	if (slotBala == -1) return; // Si están las 5 balas volando, no dispara
	
	// Elige un enemigo al azar
	int indiceCandidato = rand() % MAX_ENEMIGOS;
	if (enemigos[indiceCandidato] != NULL && enemigos[indiceCandidato]->isActivo()) {
		delete balasEnemigas[slotBala];
		balasEnemigas[slotBala] = new ProyectilEnemigo(
													   enemigos[indiceCandidato]->getX(), 
													   enemigos[indiceCandidato]->getY() + 1
													   );
	}
}
int main() {
	srand(time(NULL));
	_setcursortype(_NOCURSOR); // Oculta el cursor de la consola
	
	// Inicializar entidades
	Jugador nave(40, 22);
	nave.dibujar();
	
	Proyectil* balas[MAX_BALAS] = { NULL };
	ProyectilEnemigo* balasEnemigas[MAX_BALAS_ENEMIGAS] = { NULL };
	
	// --- DECLARACIÓN Y CREACIÓN DE ENEMIGOS ---
	Enemigo* enemigos[MAX_ENEMIGOS]; // Arreglo declarado
	
	// Configuración de cada fila: {Color, Forma, Vida/Resistencia}
	int colores[FILAS_ENEMIGOS]     = { RED, LIGHTGREEN, BLUE };
	char formas[FILAS_ENEMIGOS]     = { 'M', 'W', 'v' };
	int resistencias[FILAS_ENEMIGOS] = { 3, 2, 1 }; 
	
	int indice = 0;
	for (int fila = 0; fila < FILAS_ENEMIGOS; fila++) {
		for (int col = 0; col < COLS_ENEMIGOS; col++) {
			int posX = 10 + (col * 5); // 5 espacios entre cada enemigo
			int posY = 3 + (fila * 2); // 2 líneas de diferencia entre filas
			
			// Instanciación con los parámetros correctos
			enemigos[indice] = new Enemigo(posX, posY, colores[fila], formas[fila], resistencias[fila]);
			enemigos[indice]->dibujar();
			indice++;
		}
	}
	
	int direccion = 1; // 1 = Derecha, -1 = Izquierda
	clock_t tempoEnemigos = clock();
	clock_t pasoEnemigos = CLOCKS_PER_SEC / 2; // Se mueven cada medio segundo
	
	bool jugando = true;
	while (jugando) {
		
		// DETECCIÓN DE TECLAS (JUGADOR)
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
		
		// ACTUALIZACIÓN DE PROYECTILES 
		for (int i = 0; i < MAX_BALAS; i++) {
			if (balas[i] != NULL) {
				balas[i]->mover();
			}
		}
		
		verificarColisiones(balas, enemigos);
		// ACTUALIZACIÓN DE PROYECTILES ENEMIGOS 
		for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++) {
			if (balasEnemigas[i] != NULL) balasEnemigas[i]->mover();
		}
		
		verificarColisiones(balas, enemigos);
		
		// COLISIÓN BALA ENEMIGA VS NAVE
		if (verificarColisionJugador(balasEnemigas, nave)) {
			jugando = false; // Pierdes si te impactan
		}
		
		// MOVIMIENTO AUTOMÁTICO DE ENEMIGOS EN BLOQUE 
		if (clock() >= tempoEnemigos + pasoEnemigos) {
			bool cambiarDireccion = false;
			
			// ¿Alguno tocó la pared lateral?
			for (int i = 0; i < MAX_ENEMIGOS; i++) {
				if (enemigos[i]->isActivo()) {
					if ((enemigos[i]->getX() >= bordeDer - 2 && direccion == 1) ||
						(enemigos[i]->getX() <= bordeIzq + 2 && direccion == -1)) {
						cambiarDireccion = true;
						break;
					}
				}
			}
			
			// Si tocó la pared bajan 1 fila, si no avanzan a los costados
			int dx = cambiarDireccion ? 0 : direccion;
			int dy = cambiarDireccion ? 1 : 0;
			if (cambiarDireccion) direccion *= -1; // Invierte el sentido
			
			for (int i = 0; i < MAX_ENEMIGOS; i++) {
				enemigos[i]->moverPosicion(dx, dy);
			}
			generarDisparoEnemigo(enemigos, balasEnemigas);
			tempoEnemigos = clock();
		}
	}
	
	// LIBERACIÓN DE MEMORIA AL SALIR
	for (int i = 0; i < MAX_BALAS; i++) {
		delete balas[i];
	}
	for (int i = 0; i < MAX_BALAS_ENEMIGAS; i++) {
		delete balasEnemigas[i];
	}
	for (int i = 0; i < MAX_ENEMIGOS; i++) {
		delete enemigos[i];
	}
	
	return 0;
}
