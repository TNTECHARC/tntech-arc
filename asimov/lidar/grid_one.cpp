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
    std::vector<float> grid_;
    std::vector<bool> path_;
public:
    Grid( float resolution=.03, int size=100 ){
    size_ = size;            //THE NUMBER OF ELEMENTS ALONG AN AXIS OF THE GRID
    resolution_ = resolution;       //THE SIZE OF EACH GRID CELL IN METERS
    grid_ = std::vector<float>( size_ * size_, 0.0 );
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
            set_cell(gridX + x, gridY + y, 1000);
            set_cell(gridX + x, gridY - y, 1000);
            set_cell(gridX + x, gridY + y - 1, 1000);
            set_cell(gridX + x, gridY - y + 1, 1000);
        }
        for(y = -radius; y <= radius; y++){
            x = (int) (sqrt(r2 - y*y) - 0.5);
            set_cell(gridX + x, gridY + y, 1000);
            set_cell(gridX - x, gridY + y, 1000);
            set_cell(gridX + x - 1, gridY + y, 1000);
            set_cell(gridX - x + 1, gridY + y, 1000);
        }
    }

    void weight_cell(int sizeX, int xf, int yf){ //WEIGHTS THE GRID ACCORDING TO A GRID LOCATION
        for(int i = 0; i < sizeX * sizeX; i++)
        {
            grid_[i] = (float)sqrt(pow(xf - (i % size_), 2) + pow(yf - (i / size_),2));
        }

    }
    void compare_cell(int i, int iteration) //RECURSIVE FN THAT SETS ELEMENTS OF PATH_ TO TRUE BASED ON WEIGHTING OF GRID_
    {
        int next,last,iteration_;
        float a,b,c,d,e,f,g,h,min;
        iteration_ = iteration;
        last = i;
           //STORES WEIGHTS OF NEARBY CELLS STARTING FROM LEFT
        if(i - 1 >=0 && (i - 1) % size_ != size_ - 1)
        {
            a = grid_[i - 1];
        }
        else{ a = 1000;}
        if(i + size_ - 1 <= size_ * size_ && (i - 1) % size_ != size_ - 1)
        {
            b = grid_[i + size_ - 1];
        }
        else{b = 1000;}
        if(i + size_ <= size_ * size_)
        {
            c = grid_[i + size_];
        }
        else{c = 1000;}
        if(i + size_ + 1 <= size_ * size_ && (i + 1) % size_ != 0)
        {
            d = grid_[i + size_ + 1];
        }
        else{d = 1000;}
        if(i + 1 <= size_ * size_ && (i + 1) % size_ != 0)
        {
            e = grid_[i + 1];
        }
        else{e = 1000;}
        if(i + 1 - size_ >= 0 && (i + 1) % size_ != 0)
        {
            f = grid_[i - size_ + 1];
        }
        else{ f = 1000;}
        if(i - size_ >= 0)
        {
            g = grid_[i - size_];
        }
        else{g = 1000;}
        if(i - size_ - 1 >= 0 && (i - 1) % size_ != size_ - 1)
        {
            h = grid_[i - size_ - 1];
        }
        else{h = 1000;}

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

        grid_[last] += 100; //LESSENS LIKELYHOOD OF BACKTRACKING

        if(next != 0 && iteration_ < 200) //IF NEXT CELL ISNT DESTINATION
        {
            if(path_[next] != true) //IF NOT BACKTRACKING
            {
                path_[next] = true; //SETS ELEMENT OF PATH GRID SIMILAR TO grid_ TO PATHED
            }
            else
            {
                grid_[next] += 100; //IF BACKTRACKING REDUCE CHANCE OF REVISITING
                //path_[next] = false;
                //path_[last] = false; // CLEAR BACKTRACKED PATH
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
                    if(grid_[get_i(j,i)] != 1000 && grid_[get_i(j,i)] > 1 )
                    {
                        if(false) //SHOW WEIGHTS
                        {
                            int c = grid_[get_i(j,i)];
                            while( c >= 10)
                            {
                                c /= 10;
                            }
                            std::cout << c;
                        }
                        else
                            std::cout << ".";
                        }
                    else if( grid_[get_i(j,i)] == 1000 )
                    {
                        std::cout << "#";
                    }
                    else{ std::cout << "X";}
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
    int radius = 16;
    grid_one.weight_cell(size_,destX,destY);

    for( int i = 0; i < 5; i++ )  //MAIN LOOP THROUGH LIDARS ANGLE RANGE
      {
        float x = ((rand() % 100) - 50) * 0.03;
        float y = 0.5 + (rand() % 100) * 0.03;
        grid_one.pad_path( CartPoint( x, y ), radius);
      }

    grid_one.compare_cell(50,0); //STARTS LISTING PATHS IN path_ STARTING AT i = 50. **NEEDS DEBUGGED**

    grid_one.print();   //TEST DATA DEBUG OUTPUT

    return 0;
}
/*******************************************************************
TODO: INTERPOLATE PATH_ INTO A DIRECTION







************************************************************************/
