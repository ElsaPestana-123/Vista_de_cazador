#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
using namespace std;

// estructuras necesarias

double distancia(int x1, int y1, int x2, int y2); // firma de distancia

struct pixel{ // cada pixel tiene una coordenada (x,y) y su color 
    int x,y;

    int valor;
};

struct Imagen {

    string formato; // P2

    int ancho, alto; // ancho y alto de la imagen

    int maxValorColor; // color maximo 

    int pixeles[256][256];  //maximo 256x256 pixeles
};

struct Region { // una region es un circulo.

    pixel* pixeles; 

    int cantidad; // cantidad de pixeles en la region
};

void direcciones(int dir, int &x, int &y){

    switch(dir){

        case 0: x = 0; y = -1; // arriba
        break;

        case 1: x = 0; y = 1; // abajo
        break;

        case 2: x = 1; y = 0; // derecha
        break;

        case 3: x = -1; y = 0; // izquierda
        break;
    }

}

bool esBorde(Imagen &imagen, int x, int y){

    if(x <= 0 || x >= imagen.ancho -1 || y <= 0 || y >= imagen.alto -1) return true;   

    if (imagen.pixeles[y-1][x] == 0) return true;  // arriba
  
    if (imagen.pixeles[y+1][x] == 0) return true;  // abajo
    
    if (imagen.pixeles[y][x-1] == 0) return true; // izquierda 
    
    if (imagen.pixeles[y][x+1] == 0) return true; // derecha 
    
    return false;
}

class Circulo {

    public:
        int id;
        int centroX;
        int centroY;
        int radio;
        int circunferencia;
        int diametro;
        int areaNormal;
        int intensidadColor;
        Region regioninfo;

        Circulo() : id(0), centroX(0), centroY(0), radio(0), 
            intensidadColor(0), circunferencia(0), 
            diametro(0), areaNormal(0){

        }

        void calcularCentro() {

            if(regioninfo.cantidad == 0) { // si el circulo no tiene pixeles
                this -> centroX = 0;
                this -> centroY = 0;
                return;
            }

            int sumaX = 0;
            int sumaY = 0;

            for(int i = 0; i < regioninfo.cantidad; i++){ 

                sumaX += regioninfo.pixeles[i].x;
                sumaY += regioninfo.pixeles[i].y;
            }

            this -> centroX = (int)(((double)sumaX / regioninfo.cantidad) + 0.5); 
            this -> centroY = (int)(((double)sumaY / regioninfo.cantidad) + 0.5);
        }

        int calcularRadio(Imagen &imagen){ // el radio es la distancia maxima desde el centro a cualquier punto de la circunferencia

            int maxDistancia = 0;

            for(int i = 0; i < regioninfo.cantidad; i++){
                pixel &pixelActual = regioninfo.pixeles[i];

                double distan = distancia(this -> centroX, this -> centroY, pixelActual.x, pixelActual.y); 
                int distanciaActual = (int)distan + 1; // redondeo hacia arriba

                if(distanciaActual > maxDistancia) maxDistancia = distanciaActual;
            }

            if(maxDistancia == 0) maxDistancia = 1; // si el circulo es de un pixel, su radio es 1

            return this -> radio = maxDistancia;
        }

        int calcularCircunferencia(Imagen &imagen){

            int contadorBorde = 0;

            for(int i = 0; i < regioninfo.cantidad; i++){

                pixel &pixelActual = regioninfo.pixeles[i];

                if(esBorde(imagen, pixelActual.x, pixelActual.y)){ // se verifica si un pixel es borde

                    contadorBorde++;
                }
            }
            return this -> circunferencia = contadorBorde;
        }

        int calcularDiametro(int radio){
           return 2 * radio - 1; // el diametro se calcula como 2*radio -1
        }
}; 

// funciones de dectcion de circulos

void estanque(int x, int y, Imagen &imagen, Region &region, bool visitado[256][256] ){ 

    if(imagen.pixeles[y][x] == 0) return; // si no hay color, retorna

    int maxP = imagen.ancho * imagen.alto;

    int* ondaActualx = new int[maxP]; // onda de pixeles actuales en x
    int* ondaActualy = new int[maxP]; // onda de pixeles actuales en y
    int* ondaSiguientex = new int[maxP]; // onda de pixeles siguientes en x
    int* ondaSiguientey = new int[maxP]; // onda de pixeles siguientes en y

    int tamOndaActual = 0;
    int tamOndaSiguiente = 0;

    int colorObjetivo = imagen.pixeles[y][x]; // color del primer pixel que no sea 0

    ondaActualx[tamOndaActual] = x; // el primer pixel se agrega a la onda actual (x)

    ondaActualy[tamOndaActual] = y; // el primer pixel se agrega a la onda actual (y)

    tamOndaActual++;// aumenta el tamaño de la onda actual

    visitado[y][x] = true;// se marca como visitado

    while(tamOndaActual > 0){ 

        tamOndaSiguiente = 0;

        for(int i = 0; i < tamOndaActual; i++){ // Se recorren todos los pixeles de la onda actual

            int actualx = ondaActualx[i];
            int actualy = ondaActualy[i];

            // Se agregan los pixeles actuales a la region/circulo

            region.pixeles[region.cantidad].x = actualx; // coordenada x

            region.pixeles[region.cantidad].y = actualy; // coordenada y

            region.pixeles[region.cantidad].valor = imagen.pixeles[actualy][actualx]; // color del pixel

            region.cantidad++; // aumentamos la cantidad de pixeles del circulo

            for(int dir = 0; dir < 4; dir++){ // Se buscan los 4 vecinos mas cercanos del pixel actual

                int dx, dy;

                direcciones(dir, dx, dy);

                int nuevoX = actualx + dx; // nuevoX es el vecino de nuestra onda acutal

                int nuevoY = actualy + dy; // nuevoY es el vecino de nuestra onda actual

                if(nuevoX >= 0 && nuevoX < imagen.ancho && nuevoY >= 0 && nuevoY < imagen.alto){ 
                    // si el vecino esta dentro de los limites de la imagen
                    
                    if(!visitado[nuevoY][nuevoX] && imagen.pixeles[nuevoY][nuevoX] == colorObjetivo){
                        // si el vecino no ha sido visitado y tiene el mismo color que el objetivo

                        ondaSiguientex[tamOndaSiguiente] = nuevoX; // se agrega a la onda siguiente (x)

                        ondaSiguientey[tamOndaSiguiente] = nuevoY; // se agrega a la onda siguiente (y)

                        tamOndaSiguiente++; // aumenta el tamaño de la onda siguiente

                        visitado[nuevoY][nuevoX] = true; // se marca como visitado
                    }
                }

            }

        }

        tamOndaActual = tamOndaSiguiente; // la onda actual ahora es la siguiente

        for(int j = 0; j < tamOndaSiguiente; j++){

            ondaActualx[j] = ondaSiguientex[j]; 

            ondaActualy[j] = ondaSiguientey[j];
        }    

    }

    // liberar memoria

    delete[] ondaActualx;
    delete[] ondaActualy;
    delete[] ondaSiguientex;
    delete[] ondaSiguientey;
}

void detectarCirculos(Imagen &imagen, Circulo circulos[32], int &totalCirculos, int m){

    bool visitado[256][256] = {false}; // matriz de pixeles visitados

    totalCirculos = 0;

    for(int y = 0; y < imagen.alto; y++){
        for(int x = 0; x < imagen.ancho; x++){

            if(imagen.pixeles[y][x] != 0 && !visitado[y][x]){ // si el pixel es diferente de 0 y no ha sido visitado

                int colorActual = imagen.pixeles[y][x]; // color del pixel actual

                Circulo &circuloActual = circulos[totalCirculos]; 

                circuloActual.regioninfo.cantidad = 0; 

                circuloActual.regioninfo.pixeles = new pixel[imagen.ancho * imagen.alto];

                estanque(x, y, imagen, circuloActual.regioninfo, visitado); // detectamos la region/circulo

                if(!(circuloActual.regioninfo.cantidad > 0)){ // si no se detecto ningun pixel en el circulo

                    delete[] circuloActual.regioninfo.pixeles; // liberar memoria

                } else { 

                  // Se calculan las propiedades del circulo 

                  circuloActual.id = totalCirculos + 1;

                  circuloActual.calcularCentro();

                  circuloActual.calcularRadio(imagen);

                  circuloActual.diametro = circuloActual.calcularDiametro(circuloActual.radio);

                  circuloActual.calcularCircunferencia(imagen); 

                  circuloActual.intensidadColor = colorActual;
                
                   totalCirculos++;

                   if(totalCirculos == m) return; // si ya se alcanzaron los m circulos, se retorna

                   if(totalCirculos >= 32) return; // si ya se alcanzaron los 32 circulos maximos, se retorna
                } 

            }
        }
    }
}

// funciones de calculo

double distancia(int x1, int y1, int x2, int y2){ 
    int x = x2 - x1;
    int y = y2 - y1;
    return sqrt(x * x + y * y);

}

double PiIndividual(Circulo &circulo){
    if(circulo.diametro == 0) return 0.0; 
    return (double)circulo.circunferencia / circulo.diametro;
}

double calcularPiNormalizado(Circulo circulos[], int totalCirculos){
    double sumaPi = 0.0;

    for(int i = 0; i < totalCirculos; i++){
        sumaPi += PiIndividual(circulos[i]);
    }
    return sumaPi / totalCirculos;

}

int calcularAreaNormal(double piNormalizado, int radio){
    double areaCirculo = piNormalizado * radio * radio;
    return (int)ceil(areaCirculo);
}

void calcularAreasNormalizadas(Circulo circulos[], int totalCirculos){

    double piNormalizado = calcularPiNormalizado(circulos, totalCirculos);

    for(int i = 0; i < totalCirculos; i++){
        circulos[i].areaNormal = calcularAreaNormal(piNormalizado, circulos[i].radio);
    }
}

// funciones de lectura 

void lecturaImagen(string &nombreArchivo, Imagen &imagen){

    ifstream archivo(nombreArchivo);

    if(!archivo.is_open()){
        cerr << "Error al abrir el archivo de imagen." << endl;
        return;
    }

    archivo >> imagen.formato;

    archivo >> imagen.ancho >> imagen.alto;
    archivo >> imagen.maxValorColor;

    for(int i = 0; i < imagen.alto; i++)
        for(int j = 0; j < imagen.ancho; j++)
            archivo >> imagen.pixeles[i][j];

    archivo.close();
}

void escrituraImagen(Circulo circulos[], int totalCirculos, Imagen &imagen){

    int matriz[256][256] = {0};
    int colorCentro_New = imagen.maxValorColor;

    for(int y = 0; y < imagen.alto; y++){
        for(int x = 0; x < imagen.ancho; x++){
            if(imagen.pixeles[y][x] != 0 && esBorde(imagen, x, y)){
                matriz[y][x] = imagen.pixeles[y][x];
            } 
        }
    }  
    
    for(int i = 0; i < totalCirculos; i++){

        Circulo circulos_actual = circulos[i];
        if(circulos_actual.centroX >= 0 && circulos_actual.centroX < imagen.ancho // esto se hace para evitar accesos fuera de rango
        && circulos_actual.centroY >= 0 && circulos_actual.centroY < imagen.alto) {

            matriz[circulos_actual.centroY][circulos_actual.centroX] = colorCentro_New;
        }
           
    }

    ofstream archivoS("sites.pgm");

    if(!archivoS.is_open()){
        cerr << "Error al abrir el archivo de salida." << endl;
        return;
    }

    archivoS << imagen.formato << endl;
    archivoS << imagen.ancho << " " << imagen.alto << endl;
    archivoS << imagen.maxValorColor << endl;
    for(int i = 0; i < imagen.alto; i++){
        for(int j = 0; j < imagen.ancho; j++){
            
            archivoS << matriz[i][j] << "  ";
            if(j == imagen.ancho -1) {
                archivoS << " ";
            } 
        }
        archivoS << endl;
    }
    archivoS.close();
}

// orden de circulos por color

void ordenarCirculosPorColor(Circulo circulos[], int totalCirculos){
    for(int i = 0; i < totalCirculos -1; i++){
        for(int j = 0; j < totalCirculos -i -1; j++){
            if(circulos[j].intensidadColor > circulos[j+1].intensidadColor){
                Circulo temp = circulos[j];
                circulos[j] = circulos[j+1];
                circulos[j+1] = temp;
            }
        }
    }
}

// funcion backtracking
bool conjuntoWin_ciruclos[32];
int bestSuma_circulos = -1; // indica que la mejor suma encontrada es -1 (no se ha encontrado ninguna)
int bestCantidad_circulos = -1; // indica que la mejor cantidad de circulos encontrados es -1 (no se ha encontrado ninguno)
int MasOscuro_circulos = -1; // indica que la mejor intensidad de color es -1 (no se ha encontrado ninguno)

void mochilero_circulos(Circulo circulos[], int totalCirculos, int T, int index, int 
    sumaActual, int cantidadActual, int IOactual, bool conjuntoActual[]){ // perdon por tantos parametros

    // La mejor solucion encontrada hasta ahora

    if(sumaActual <= T){
        bool bestSolution = false; // evaluador para las condiones siguientes a cumplir

        if(sumaActual > bestSuma_circulos){ // 
            bestSolution = true;

        } else if(sumaActual == bestSuma_circulos){ // si la suma que tengo es igual a la mejor suma hasta ahora

            if(cantidadActual < bestCantidad_circulos){ // agarrar la lista con menores elementos
                bestSolution = true;
                
            } else if(cantidadActual == bestCantidad_circulos){ 

                if(IOactual < MasOscuro_circulos){ // agarrar la lista con circulos mas oscuros
                    bestSolution = true;
                }
            }
        }

        if(bestSolution){ // actualizar datos

            bestSuma_circulos = sumaActual;
            bestCantidad_circulos = cantidadActual;
            MasOscuro_circulos = IOactual;

            for(int i = 0; i < totalCirculos; i++){
                conjuntoWin_ciruclos[i] = conjuntoActual[i];
            }
        }
        

    }

    if(index == totalCirculos || sumaActual > T){ // si ya revisamos todos los circulos o nos pasamos de T
        return;
    }

    if(sumaActual + circulos[index].areaNormal <= T){ // si la suma actual mas el area normalizada del circulo actual es menor o igual a T lo incluimos
       conjuntoActual[index] = true;

       int newIO;
       if(cantidadActual == 0) newIO = index;
       else newIO = IOactual;

       mochilero_circulos(circulos, totalCirculos, T, index+1, 
        sumaActual + circulos[index].areaNormal, cantidadActual+1, newIO, conjuntoActual);

    }   

    conjuntoActual[index] = false; // backtracking

    mochilero_circulos(circulos, totalCirculos, T, index+1, sumaActual, cantidadActual, IOactual, conjuntoActual);
 

}

int main() {

    int N,M,T;
    cin >> N >> M >> T;

    Imagen imagen;
    string nombreArchivo = "objectives" + to_string(N) + ".pgm";

    lecturaImagen(nombreArchivo, imagen);

    Circulo circulos[32]; // maximo 32 circulos detectados

    int totalCirculos = 0;

    detectarCirculos(imagen, circulos, totalCirculos, M);


    calcularAreasNormalizadas(circulos, totalCirculos);


    ordenarCirculosPorColor(circulos, totalCirculos);

    for(int i = 0; i < totalCirculos; i++){
        Circulo circuloActual = circulos[i];
        cout << circuloActual.areaNormal << "u" << endl;
    }

    cout << endl;

    bestSuma_circulos = -1;
    bestCantidad_circulos = -1;
    MasOscuro_circulos = -1;

    for(int i=0; i < totalCirculos; i++) {
        conjuntoWin_ciruclos[i] = false;
    }

    bool seleccionTemporal[32] = {false};

    mochilero_circulos(circulos, totalCirculos, T, 0, 0, 0, -1, seleccionTemporal);

    bool primero = true;

    for(int i = 0; i < totalCirculos; i++) {

        if(conjuntoWin_ciruclos[i] == true) {

            if(!primero) cout << ","; 

            cout << "Circulo" << circulos[i].id;

            primero = false;
        }
    }
    cout << endl; 

    escrituraImagen(circulos, totalCirculos, imagen);
    
    return 0;
}