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
    std::vector<int> grid_;
    std::vector<bool> path_;
public:
    Grid( float resolution=.03, int size=100 ){
    size_ = size;            //THE NUMBER OF ELEMENTS ALONG AN AXIS OF THE GRID
    resolution_ = resolution;       //THE SIZE OF EACH GRID CELL IN METERS
    grid_ = std::vector<int>( size_ * size_, 0 );
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
            y = (int) (sqrt(r2 - x*x));
            set_cell(gridX + x, gridY + y, 9999);
            set_cell(gridX + x, gridY - y, 9999);
            set_cell(gridX + x, gridY + y - 1, 9999);
            set_cell(gridX + x, gridY - y + 1, 9999);
        }
        radius--;
        r2 = radius * radius;
        for(x = -radius; x <= radius; x++){
            y = (int) (sqrt(r2 - x*x));
            set_cell(gridX + x, gridY + y, 9999);
            set_cell(gridX + x, gridY - y, 9999);
            set_cell(gridX + x, gridY + y - 1, 9999);
            set_cell(gridX + x, gridY - y + 1, 9999);
        }
    }

    void weight_cell(int sizeX, int xf, int yf){ //WEIGHTS THE GRID ACCORDING TO A GRID LOCATION
        for(int i = 0; i < sizeX * sizeX; i++)
        {
            grid_[i] = abs(xf - (i % size_)) + abs(yf - (i / size_));
        }

    }
    void compare_cell(int i, int iteration) //RECURSIVE FN THAT SETS ELEMENTS OF PATH_ TO TRUE BASED ON WEIGHTING OF GRID_
    {
        int min,a,b,c,d,e,f,g,h,next,last,iteration_;
        iteration_ = iteration;
        last = i;
           //STORES WEIGHTS OF NEARBY CELLS STARTING FROM LEFT
        if(i - 1 >=0)
        {
            a = grid_[i - 1];
        }
        else{ a = 9000;}
        if(i + size_ - 1 <= size_ * size_)
        {
            b = grid_[i + size_ - 1];
        }
        else{b = 9000;}
        if(i + size_ <= size_ * size_)
        {
            c = grid_[i + size_];
        }
        else{c = 9000;}
        if(i + size_ + 1 <= size_ * size_)
        {
            d = grid_[i + size_ + 1];
        }
        else{d = 9000;}
        if(i + 1 <= size_ * size_)
        {
            e = grid_[i + 1];
        }
        else{e = 9000;}
        if(i + 1 - size_ >= 0)
        {
            f = grid_[i - size_ + 1];
        }
        else{ f = 9000;}
        if(i - size_ >= 0)
        {
            g = grid_[i - size_];
        }
        else{g = 9000;}
        if(i - size_ - 1 >= 0)
        {
            h = grid_[i - size_ - 1];
        }
        else{h = 9000;}

        min = a;    //SETS MIN TO LOWEST ELEMENT OF GRID_
        if( min > b){min = b;}
        if( min > c){min = c;}
        if( min > d){min = d;}
        if( min > e){min = e;}
        if( min > f){min = f;}
        if( min > g){min = g;}
        if( min > h){min = h;}

        next = 0;
        if(min==a){next = i -1;}    //SETS NEXT TO THE 1D ADDRESS OF MIN
        if(min==b){next = i + size_ - 1;}
        if(min==c){next = i + size_;}
        if(min==d){next = i + size_ + 1;}
        if(min==e){next = i + 1;}
        if(min==f){next = i + 1 - size_;}
        if(min==g){next = i + -size_;}
        if(min==h){next = i + -size_ - 1;}
        if(min==0){next = 0;}

        grid_[last] += 10; //LESSENS LIKELYHOOD OF BACKTRACKING

        if(next != 0 && iteration_ < 500) //IF NEXT CELL ISNT DESTINATION
        {
            if(path_[next] != true) //IF NOT BACKTRACKING
            {
                path_[next] = true; //SETS ELEMENT OF PATH GRID SIMILAR TO grid_ TO PATHED
            }
            else
            {
                grid_[next] += 10; //IF BACKTRACKING REDUCE CHANCE OF REVISITING
                path_[next] = false; // CLEAR BACKTRACKED PATH
            }

            iteration_++;
            compare_cell(next, iteration_); //PROCESSES NEXT POINT
        }
        else
        {
            path_[next] = true; //SETS DESTINATION TO PATHED
        }
    }

    void print() //DEBUG PRINTOUT
    {
        for( int i = size_ - 1; i >= 0; i--)
        {
            for( int j = 0; j < size_; j++)
            {   if(path_[get_i(j,i)] == true)
                {
                    std::cout << "!";
                }
                else
                {
                    if(grid_[get_i(j,i)] != 9999 && grid_[get_i(j,i)] > 1 )
                    {   /*
                        int c = grid_[get_i(j,i)];
                        while( c >= 10)
                        {
                            c /= 10;
                        }
                        std::cout << c;
                        */
                        std::cout << ".";
                        }
                    else if( grid_[get_i(j,i)] == 9999 )
                    {
                        std::cout << "#";
                    }
                    else{ std::cout << 0;}
                }
            }
            std::cout << "\n";
        }
    }
};

int main(){
    Grid grid_one;

    srand(time(NULL));
    //float theta[100];
    //float r[100];
    int size_ = 100;
    float dir = -25;    //DIRECTION OF WAYPOINT
    float dirRad = -1 * (dir + 270) * 0.0174532925; //CONVERT TO RADIANS, ADJUST QUADRANT
    int destX = 50 + (size_ * cos(dirRad)); //START FROM CENTER OF X AXIS RATHER THAN 0
    int destY = (size_ * sin(dirRad));
    if(destX < 0){destX = 0;}
    if(destX > 100){destX = 100;}
    if(destY < 0){destY = 0;}
    if(destY > 100){destY = 100;}

    grid_one.weight_cell(size_,destX,destY);

    for( int i = 0; i < 50; i++ )  //MAIN LOOP THROUGH LIDARS ANGLE RANGE
      {
        //float x = cos( theta[i]) * r[i];    //POLAR TO CARTESIAN CONVERSION
        //float y = sin( theta[i]) * r[i];
        float x = ((rand() % 100) - 50) * 0.06; //DEBUG TEST DATA
        float y = (rand() % 100) * 0.03;
        grid_one.pad_path( CartPoint( x, y ), 8);
      }

    grid_one.compare_cell(50,0); //STARTS LISTING PATHS IN path_ STARTING AT i = 50. **NEEDS DEBUGGED**

    grid_one.print();   //TEST DATA DEBUG OUTPUT

    return 0;
}
/*******************************************************************
TODO: DEBUG COMPARE_CELL







************************************************************************/
