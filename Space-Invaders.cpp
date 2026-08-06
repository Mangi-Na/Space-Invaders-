#include <iostream>
#include <conio2.h> 

using namespace std;

const int bordeIzq = 1;
const int bordeDer = 119;



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
		cout << ' '; // "borrar" la nave de su lugar viejo
	}
	
	virtual void dibujar() = 0; // Cada objeto elegirá cómo dibujarse
	
	int getX() const { return x; }
	int getY() const { return y; }
};

//NAVE(Jugador)
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
		
	Jugador nave(40, 22); // Creamos la nave en el centro abajo
	nave.dibujar();
	
	bool jugando = true;
	while (jugando) {
		if (kbhit()) { // Si tocás una tecla...
			char tecla = getch();
			if (tecla == 'a' || tecla == 'A') nave.moverIzquierda();
			if (tecla == 'd' || tecla == 'D') nave.moverDerecha();
			if (tecla == 27) jugando = false; // Tecla ESC para salir
		}
	}
	
	return 0;
}
