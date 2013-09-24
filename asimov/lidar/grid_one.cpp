//TTUARC 9/21/13 LIDAR GRID PROGRAM
//POORLY SIMULATES LIDAR DATA, PLOTS DATA AS CIRCLES TO GRID TO DENOTE OBSTACLES
//WEIGHTS GRID ACCORDING TO DISTANCE FROM DESTINATION DETERMINED BY DIRECTION
//MAKES PATH ALONG GRID CELLS TO DESTINATION

#include <vector>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>

class CartPoint{
public:
  CartPoint( float x, float y )
  { x_ = x;
    y_ = y;
  }
  float x_;
  float y_;
};

class Grid{

private:
    float resolution_;
    int size_;
    std::vector<bool> grid_;
    std::vector<bool> path_;
public:
    Grid( float resolution=.03, int size=100 ){
    size_ = size;            //THE NUMBER OF ELEMENTS ALONG AN AXIS OF THE GRID
    resolution_ = resolution;       //THE SIZE OF EACH GRID CELL IN METERS
    grid_ = std::vector<bool>( size_ * size_, false );
    path_ = std::vector<bool>( size_ * size_, false);   // CONSTRUCTS PACKED INTEGER ARRAY OF SIZE_ BY SIZE_
    }

    void set_cell( int gridX, int gridY, int weight = 0 ){  //MODIFIES GRID CELL GIVEN X Y ADDRESS
        if( gridX >= 0 && gridX < size_ &&      //PROTECTS ROWS AND COLUMNS FROM BEING OVERWRITTEN
            gridY >= 0 && gridY < size_ ){
            int i = gridX + gridY * size_;
            grid_[i] = weight;
        }
    }

    int get_i( int x, int y ){ //RETURNS 1D ADDRESS GIVEN 2D ADDRESS
      return x + y*size_;
    }

    void pad_path(CartPoint point, int radius) //SETS CELLS AS OBSTRUCTED IN A RADIUS AROUND A CART. POINT
    {
        int gridX = point.x_/resolution_ + size_/2;
        int gridY = point.y_/resolution_;
        int x, y, r2;
        r2 = radius * radius;
        for (x = -radius; x <= radius; x++) {
            y = (int) (sqrt(r2 - x*x) + 0.5);
            set_cell(gridX + x, gridY + y, true);
            set_cell(gridX + x, gridY - y, true);
            set_cell(gridX + x, gridY + y - 1, true);
            set_cell(gridX + x, gridY - y + 1, true);
            set_cell(gridX + x, gridY + y - 2, true);
            set_cell(gridX + x, gridY - y + 2, true);
        }
        for(y = -radius; y <= radius; y++){
            x = (int) (sqrt(r2 - y*y) - 0.5);
            set_cell(gridX + x, gridY + y, true);
            set_cell(gridX - x, gridY + y, true);
            set_cell(gridX + x - 1, gridY + y, true);
            set_cell(gridX - x + 1, gridY + y, true);
            set_cell(gridX + x - 2, gridY + y, true);
            set_cell(gridX - x + 2, gridY + y, true);
        }
    }
    void compare_cell(int i, int xf, int yf, int iteration) //RECURSIVE FN THAT SETS ELEMENTS OF PATH_ TO TRUE BASED ON WEIGHTING OF GRID_
    {
        int next,last,iteration_;
        int a,b,c,d,e,f,g,h,min;
        iteration_ = iteration;
           //STORES WEIGHTS OF NEARBY CELLS STARTING FROM LEFT
        if(i - 1 >=0 && (i - 1) % size_ != size_ - 1) //IF CELL ADDRESS IS NOT WRAPPED AROUND X OR BEYOND THE SCOPE OF THE ARRAY
        {
            a = i - 1; //CELL ADDRESS TO THE LEFT
            if(grid_[a] != true){ //IF NOT AN OBSTACLE OR BOUNDARY
                a = sqrt(pow(xf - (a % size_),2) + pow(yf - (a / size_),2)); //SET a TO BLOCK DISTANCE FROM DESTINATION
            }
            else{ a = 2000;}
        }
        else{ a = 2000;}
        if(i + size_ - 1 <= size_ * size_ && (i - 1) % size_ != size_ - 1)
        {
            b = i + size_ - 1; //FORWARD LEFT
            if(grid_[b] != true){
                b = sqrt(pow(xf - (b % size_),2) + pow(yf - (b / size_),2));
            }
            else{b = 2000;}
        }
        else{b = 2000;}
        if(i + size_ <= size_ * size_)
        {
            c = i + size_; //FORWARD
            if(grid_[c] != true){
                c = sqrt(pow(xf - (c % size_),2) + pow(yf - (c / size_),2));
            }
            else{c = 2000;}
        }
        else{c = 2000;}
        if(i + size_ + 1 <= size_ * size_ && (i + 1) % size_ != 0)
        {
            d = i + size_ + 1; //FORWARD RIGHT
            if(grid_[d] != true){
                d = sqrt(pow(xf - (d % size_),2) + pow(yf - (d / size_),2));
            }
            else{d = 2000;}
        }
        else{d = 2000;}
        if(i + 1 <= size_ * size_ && (i + 1) % size_ != 0)
        {
            e = i + 1; //RIGHT
            if(grid_[e] != true){
                e = sqrt(pow(xf - (e % size_),2) + pow(yf - (e / size_),2));
            }
            else{e = 2000;}
        }
        else{e = 2000;}
        if(i + 1 - size_ >= 0 && (i + 1) % size_ != 0)
        {
            f = i - size_ + 1; //BACKWARD RIGHT
            if(grid_[f] != true){
                f = sqrt(pow(xf - (f % size_),2) + pow(yf - (f / size_),2));
            }
            else{f = 2000;}
        }
        else{ f = 2000;}
        if(i - size_ >= 0)
        {
            g = i - size_; //BACKWARD
            if(grid_[g] != true){
                g = sqrt(pow(xf - (g % size_),2) + pow(yf - (g / size_),2));
            }
            else{g = 2000;}
        }
        else{g = 2000;}
        if(i - size_ - 1 >= 0 && (i - 1) % size_ != size_ - 1)
        {
            h = i - size_ - 1; //BACKWARD LEFT
            if(grid_[h] != true){
                h = sqrt(pow(xf - (h % size_),2) + pow(yf - (h / size_),2));
            }
            else{h = 2000;}
        }
        else{h = 2000;}

        min = a;    //SETS MIN TO CHEAPEST CELL
        if( min > b){min = b;}
        if( min > c){min = c;}
        if( min > d){min = d;}
        if( min > e){min = e;}
        if( min > f){min = f;}
        if( min > g){min = g;}
        if( min > h){min = h;}

        next = i;
        if(min==a){next = i -1;}    //SETS NEXT TO THE 1D ADDRESS OF MIN
        if(min==b){next = i + size_ - 1;}
        if(min==c){next = i + size_;}
        if(min==d){next = i + size_ + 1;}
        if(min==e){next = i + 1;}
        if(min==f){next = i + 1 - size_;}
        if(min==g){next = i + -size_;}
        if(min==h){next = i + -size_ - 1;}
        if(min==0){next = i;}

        grid_[i] = true; //PREVENT BACKTRACKING

        if(next != i && iteration_ < 200) //IF NEXT CELL ISNT DESTINATION AND LESS THAN 200 ITERATIONS
        {
            if(path_[next] != true) //IF NOT BACKTRACKING
            {
                path_[next] = true; //SETS ELEMENT OF PATH GRID SIMILAR TO grid_ TO PATHED
            }
            iteration_++;
            compare_cell(next, xf, yf, iteration_); //PROCESSES NEXT POINT
        }
        else
        {
            path_[next] = true; //SETS DESTINATION TO PATHED
        }
    }

    void print(int xf, int yf) //DEBUG PRINTOUT
    {
        for( int i = size_ - 1; i >= 0; i--)
        {
            for( int j = 0; j < size_; j++)
            {
                if( j == xf && i == yf)
                {
                    std::cout << "X";
                }
                else if(path_[get_i(j,i)] == true)
                {
                    std::cout << "!";
                }

                else
                {
                    if(grid_[get_i(j,i)] != true)
                    {
                            std::cout << ".";
                    }
                    else if( grid_[get_i(j,i)] == true )
                    {
                        std::cout << "#";
                    }
                }
            }
            std::cout << "\n";
        }
    }
};

int main(){
    Grid grid_one;

    srand(time(NULL));
    int size_ = 100;
    int destX = rand() % size_;
    int destY = 75;
    int radius = 4;

    for( int i = 0; i < 100; i++ )  //MAIN LOOP THROUGH LIDARS ANGLE RANGE
      {
        float x = ((rand() % 100) - 50) * 0.03;
        float y = 0.5 + (rand() % 100) * 0.03;
        grid_one.pad_path( CartPoint( x, y ), radius);
      }

    grid_one.compare_cell(50, destX, destY, 0); //STARTS LISTING PATHS IN path_ STARTING AT i = 50. **NEEDS DEBUGGED**

    grid_one.print(destX, destY);   //TEST DATA DEBUG OUTPUT

    return 0;
}
/*******************************************************************
TODO: INTERPOLATE PATH_ INTO A DIRECTION







************************************************************************/
