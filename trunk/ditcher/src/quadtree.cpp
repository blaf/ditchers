#include "quadtree.hpp"
#include "game.hpp"
#include "global.hpp"

QuadTreeSurface::QuadTreeSurface(SDL_Surface* src){
    source = src;
    init(0, 0, source->w, source->h);
    build();
};

QuadTreeSurface::QuadTreeSurface(SDL_Surface* src, int newx1, int newy1, int newx2, int newy2){
    source = src;
    init(newx1, newy1, newx2, newy2);
};

bool QuadTreeSurface::decide(int testx, int testy){
    return gameplay.getPoint(source, Point(testx, testy));
}

QuadTree<bool>* QuadTreeSurface::son(int newx1, int newy1, int newx2, int newy2){
    if ((newx1 == newx2) || (newy1 == newy2)) return 0;
    else return new QuadTreeSurface(source, newx1, newy1, newx2, newy2);
}

void QuadTreeSurface::paint(){
    if (modified) modified = false; else return;
    if (leaf){
        bool d = decide(x1, y1);

        int s = size();
        int col = 255;
        int m = 1;
        int i = 0;
        while (m < s) { col-=18-i; i+=1; m*=2; }
        if (value) boxRGBA(source, x1, y1, x2-1, y2-1, 0, col, col, 255);

        if (d != value){
            boxRGBA(source, x1, y1, x2-1, y2-1, d ? 255 : 0, d ? 0 : 255, 0, 255);
            printf("Surface: bad colour: %d %d %d %d\n", x1, y1, x2, y2);
        }
    }else{
        for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++){
            if (qt[i][j]) qt[i][j]->paint();
        }
    }
}


QuadTreeComposite::QuadTreeComposite(SDL_Surface* newmud, SDL_Surface* newrock){
    rock = newrock;
    mud  = newmud;
    init(0, 0, mud->w, mud->h);
    build();
};

QuadTreeComposite::QuadTreeComposite(SDL_Surface* newmud, SDL_Surface* newrock, int newx1, int newy1, int newx2, int newy2){
    rock = newrock;
    mud  = newmud;
    init(newx1, newy1, newx2, newy2);
};

int QuadTreeComposite::decide(int testx, int testy){
    Point where(testx, testy);
    if (gameplay.getPoint(rock, where)) return 2;
    else if (gameplay.getPoint(mud, where)) return 1;
    else return 0;
}

QuadTree<int>* QuadTreeComposite::son(int newx1, int newy1, int newx2, int newy2){
    if ((newx1 == newx2) || (newy1 == newy2)) return 0;
    else return new QuadTreeComposite(mud, rock, newx1, newy1, newx2, newy2);
}

void QuadTreeComposite::setcoverage(){
    QuadTree<int>::setcoverage();
    if (leaf){
        if (value == 2) rockerage = size(); else rockerage = 0;
    }else{
        rockerage = 0;
        for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) if (qt[i][j]){
            rockerage += dynamic_cast<QuadTreeComposite*>(qt[i][j])->rockerage;
        }
    }
}

int QuadTreeComposite::getrockerage(int rx1, int ry1, int rx2, int ry2){
    if (rx1 > rx2){
        return (getrockerage(rx1, ry1, x2-1, ry2) + getrockerage(x1, ry1, rx2, ry2));
    }else if(ry1 > ry2){
        return (getrockerage(rx1, ry1, rx2, y2-1) + getrockerage(rx1, y1, rx2, ry2));
    }

    if (rx1 < x1) rx1 = x1;
    if (ry1 < y1) ry1 = y1;
    if (rx2 >= x2) rx2 = x2 - 1;
    if (ry2 >= y2) ry2 = y2 - 1;

    return getrock(rx1, ry1, rx2, ry2);
}

double QuadTreeComposite::getrockratio(int rx1, int ry1, int rx2, int ry2){
    return (double)getrockerage(rx1, ry1, rx2, ry2) / size(rx1, ry1, rx2, ry2);
}

int QuadTreeComposite::getrock(int rx1, int ry1, int rx2, int ry2){

    if (leaf){
        if (value == 2) return size(rx1, ry1, rx2, ry2); else return 0;
    }else if ((rx1 == x1) && (ry1 == y1) && (rx2 == x2-1) && (ry2 == y2-1)){
        return rockerage;
    }

    int xq1 = (rx1 < xm) ? 0 : 1;
    int yq1 = (ry1 < ym) ? 0 : 1;
    int xq2 = (rx2 < xm) ? 0 : 1;
    int yq2 = (ry2 < ym) ? 0 : 1;

    if ((xq1 == xq2) && (yq1 == yq2)) return dynamic_cast<QuadTreeComposite*>(qt[xq1][yq1])->getrockerage(rx1, ry1, rx2, ry2);
    else{
        int rck = 0;
        for (int i = xq1; i <= xq2; i++) for (int j = yq1; j <= yq2; j++)
            if (qt[i][j]) rck += dynamic_cast<QuadTreeComposite*>(qt[i][j])->getrockerage(rx1, ry1, rx2, ry2);
        return rck;
    }
}

bool QuadTreeComposite::homogenous(int rx1, int ry1, int rx2, int ry2, int val){

    /**
    0 -- empty
    1 -- mud
    2 -- rock
    3 -- empty or mud
    4 -- mud or rock
    */

    if ((val >= 0) && (val <= 2)) return QuadTree<int>::homogenous(rx1, ry1, rx2, ry2, val);
    else if (val == 3) return size(rx1, ry1, rx2, ry2) == 0 ? false :
        (getrockerage(rx1, ry1, rx2, ry2) == 0);
    else if (val == 4) return size(rx1, ry1, rx2, ry2) == 0 ? true :
        (getcoverage(rx1, ry1, rx2, ry2) == size(rx1, ry1, rx2, ry2));
    else return false;

}

#include "game.hpp"

void QuadTreeComposite::paint(){ /* this method has only meaning for debugging */
    if (modified) modified = false; else return;

    if (!built) printf("Not built: %d %d %d %d\n", x1, y1, x2, y2);
    if (leaf){
        int cov = value ? size() : 0;
        if (coverage != cov) printf("Wrong LEAF  cover: %d %d %d %d: %d/%d\n", x1, y1, x2, y2, cov, coverage);

        int d = decide(x1, y1);

        int s = size();
        int col = 255;
        int m = 1;
        int i = 0;
        while (m < s) { col-=18-i; i+=1; m*=2; }
        if (value == 0) boxRGBA(gameplay.ground, x1, y1, x2-1, y2-1, 0, 0, col, 255);
        if (value == 1) boxRGBA(mud, x1, y1, x2-1, y2-1, 0, col, 0, 255);
        if (value == 2) boxRGBA(rock, x1, y1, x2-1, y2-1, col, 0, 0, 255);

        if (d != value){
            boxRGBA(mud, x1, y1, x2-1, y2-1, d ? 255 : 0, d ? 0 : 255, 0, 255);
            printf("Bad colour: %d %d %d %d\n", x1, y1, x2, y2);
        }
    }else{
        int cov = 0;
        for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++){
            if (qt[i][j]){
                qt[i][j]->paint();
                cov+=qt[i][j]->coverage;
            }
        }
        if (coverage != cov) printf("Wrong INNER cover: %d %d %d %d: %d/%d\n", x1, y1, x2, y2, cov, coverage);
    }
}

