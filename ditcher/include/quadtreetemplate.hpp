#ifndef QUADTREETEMPLATEHEADER
#define QUADTREETEMPLATEHEADER

template<class T>
class QuadTree{
    protected:

    T value;

    int x1, xm, x2;
    int y1, ym, y2;
    bool built;

    QuadTree* qt[2][2];

    bool leaf;

    bool modified; // for testing and debugging

    void build();

    void init(int newx1, int newy1, int newx2, int newy2);

    virtual QuadTree* son(int newx1, int newy1, int newx2, int newy2) = 0;

    void sons();

    void makeleaf(T val);

    virtual void setcoverage();

    void trymerge();

    virtual T decide(int testx, int testy) = 0;

    void updateRect(int rx1, int ry1, int rx2, int ry2, bool hom, T val);

    bool homog(int rx1, int ry1, int rx2, int ry2, T val);

    int getcov(int rx1, int ry1, int rx2, int ry2);

    ~QuadTree();

    public:

    int coverage;

    virtual void paint();

    int size();

    int size(int rx1, int ry1, int rx2, int ry2);

    void update(int upx, int upy);

    void updateRect(int rx1, int ry1, int rx2, int ry2);

    virtual bool homogenous(int rx1, int ry1, int rx2, int ry2, T val);

    int getcoverage(int rx1, int ry1, int rx2, int ry2);

    double getratio(int rx1, int ry1, int rx2, int ry2);
};

template<class T>
QuadTree<T>::~QuadTree(){
    if (!leaf){
        for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) if (qt[i][j]){
            delete(qt[i][j]);
            qt[i][j] = 0;
        }
    }
}

template<class T>
void QuadTree<T>::init(int newx1, int newy1, int newx2, int newy2){
    x1 = newx1; x2 = newx2;
    y1 = newy1; y2 = newy2;

    for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) qt[i][j] = 0;

    int p;
    p = 1; while (x1 + p*2 < x2) p *= 2; xm = x1 + p;
    p = 1; while (y1 + p*2 < y2) p *= 2; ym = y1 + p;

    value = decide(x1, y1);

    leaf  = true;
    built = false;

    setcoverage();
}

template<class T>
void QuadTree<T>::sons(){
    if (!leaf) return;
    if ((x2-x1==1) && (y2-y1==1)){ makeleaf(decide(x1, y1)); return; }

    leaf = false;
    qt[0][0] = son(x1, y1, xm, ym);
    if (ym != y2) qt[0][1] = son(x1, ym, xm, y2);
    if (xm != x2) qt[1][0] = son(xm, y1, x2, ym);
    if ((xm != x2) && (ym != y2)) qt[1][1] = son(xm, ym, x2, y2);
}


template<class T>
void QuadTree<T>::build(){
    T val = decide(x1, y1);
    bool split = false;

    for (int i = x1; i < x2; i++){
        for (int j = y1; j < y2; j++){
            T d = decide(i, j);
            if (d != val){
                split = true;
                break;
            }
        }
        if (split) break;
    }

    if (split){
        sons();
        for (int i = 0; i < 2; i++){
            for (int j = 0; j < 2; j++){
                if (qt[i][j]) qt[i][j]->build();
            }
        }
    }else{
        makeleaf(val);
    }

    built = true;

    setcoverage();
}

template<class T>
void QuadTree<T>::makeleaf(T val){
    value = val;
    if (!leaf){
        for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) if (qt[i][j]){
            delete(qt[i][j]);
            qt[i][j] = 0;
        }
        leaf = true;
    }
    setcoverage();
}

template<class T>
void QuadTree<T>::trymerge(){
    if (leaf) return;

    bool merge = true;

    T val = qt[0][0]->value;

    for (int i = 0; i < 2; i++){
        for (int j = 0; j < 2; j++)
            if (qt[i][j] && ((!qt[i][j]->leaf) || (qt[i][j]->value != val))) { merge = false; break; }
        if (!merge) break;
    }

    if (merge){
        built = true;
        makeleaf(val);
    }
}

template<class T>
void QuadTree<T>::setcoverage(){
    if (leaf){
        if (value) coverage = size(); else coverage = 0;
    }else{
        coverage = 0;
        for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) if (qt[i][j]){
            coverage += qt[i][j]->coverage;
        }
    }
    modified = true;
}

template<class T>
int QuadTree<T>::size(){
    return (x2-x1)*(y2-y1);
}

    /* size of intersection of arbitrary rectangle and current QT; includes bottom & right line */
template<class T>
int QuadTree<T>::size(int rx1, int ry1, int rx2, int ry2){
    if (rx1 > rx2) rx2 += x2;
    if (ry1 > ry2) ry2 += y2;
    return (rx2+1-rx1)*(ry2+1-ry1);
}

    /* testing */

template<class T>
void QuadTree<T>::paint(){}

template<class T>
void QuadTree<T>::update(int upx, int upy){
    if ((upx < 0) || (upy < 0) || (upx >= x2) || (upy >= y2)) return;
    updateRect(upx, upy, upx, upy);
}

template<class T>
void QuadTree<T>::updateRect(int rx1, int ry1, int rx2, int ry2){
    if (rx1 < x1) rx1 = x1;
    if (ry1 < y1) ry1 = y1;
    if (rx2 >= x2) rx2 = x2 - 1;
    if (ry2 >= y2) ry2 = y2 - 1;

    T val = decide(rx1, ry1);
    bool hom = true;
    for (int i = rx1; i <= rx2; i++){
        for (int j = ry1; j <= ry2; j++){
            T d = decide(i, j);
            if (d != val) { hom = false; break; }
        }
        if (!hom) break;
    }

    updateRect(rx1, ry1, rx2, ry2, hom, val);
}

template<class T>
void QuadTree<T>::updateRect(int rx1, int ry1, int rx2, int ry2, bool hom, T val){

    if (hom){
        if (built && leaf && (val == value)) return; /* no change */
        else if ((rx1 == x1) && (ry1 == y1) && (rx2 == x2-1) && (ry2 == y2-1)){
            makeleaf(val); /* change whole rectangle */
            built = true;
            return;
        }
    }

    int xq1 = (rx1 < xm) ? 0 : 1;
    int yq1 = (ry1 < ym) ? 0 : 1;
    int xq2 = (rx2 < xm) ? 0 : 1;
    int yq2 = (ry2 < ym) ? 0 : 1;

    if (leaf){
        sons();
        for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++)
            if (qt[i][j] && ((i < xq1) || (i > xq2) || (j < yq1) || (j > yq2))){
                qt[i][j]->built = true;
                qt[i][j]->setcoverage();
            }
    }

    if ((xq1 == xq2) && (yq1 == yq2)){
        qt[xq1][yq1]->updateRect(rx1, ry1, rx2, ry2, hom, val);
    }else{
        for (int i = xq1; i <= xq2; i++) for (int j = yq1; j <= yq2; j++)
            if (qt[i][j]) qt[i][j]->updateRect(rx1, ry1, rx2, ry2);
    }

    built = true;

    trymerge();

    setcoverage();
}

template<class T>
bool QuadTree<T>::homogenous(int rx1, int ry1, int rx2, int ry2, T val){

    if (rx1 > rx2){
        return (homogenous(rx1, ry1, x2-1, ry2, val) && homogenous(x1, ry1, rx2, ry2, val));
    }else if(ry1 > ry2){
        return (homogenous(rx1, ry1, rx2, y2-1, val) && homogenous(rx1, y1, rx2, ry2, val));
    }

    return homog(rx1, ry1, rx2, ry2, val);
}

template<class T>
bool QuadTree<T>::homog(int rx1, int ry1, int rx2, int ry2, T val){

    if (rx1 < x1) rx1 = x1;
    if (ry1 < y1) ry1 = y1;
    if (rx2 >= x2) rx2 = x2 - 1;
    if (ry2 >= y2) ry2 = y2 - 1;

    if (leaf){
        if (value == val) return true; else return false;
    }else if ((rx1 == x1) && (ry1 == y1) && (rx2 == x2-1) && (ry2 == y2-1)){
        return false; /* contains whole non-leaf -- cannot be homogenous */
    }

    int xq1 = (rx1 < xm) ? 0 : 1;
    int yq1 = (ry1 < ym) ? 0 : 1;
    int xq2 = (rx2 < xm) ? 0 : 1;
    int yq2 = (ry2 < ym) ? 0 : 1;

    if ((xq1 == xq2) && (yq1 == yq2)) return qt[xq1][yq1]->homog(rx1, ry1, rx2, ry2, val);
    else for (int i = xq1; i <= xq2; i++) for (int j = yq1; j <= yq2; j++)
        if (qt[i][j] && !qt[i][j]->homog(rx1, ry1, rx2, ry2, val)) return false;

    return true;
}

template<class T>
int QuadTree<T>::getcoverage(int rx1, int ry1, int rx2, int ry2){
    if (rx1 > rx2){
        return (getcoverage(rx1, ry1, x2-1, ry2) + getcoverage(x1, ry1, rx2, ry2));
    }else if(ry1 > ry2){
        return (getcoverage(rx1, ry1, rx2, y2-1) + getcoverage(rx1, y1, rx2, ry2));
    }

    return getcov(rx1, ry1, rx2, ry2);
}

template<class T>
double QuadTree<T>::getratio(int rx1, int ry1, int rx2, int ry2){
    return (double)getcoverage(rx1, ry1, rx2, ry2) / size(rx1, ry1, rx2, ry2);
}

template<class T>
int QuadTree<T>::getcov(int rx1, int ry1, int rx2, int ry2){

    if (rx1 < x1) rx1 = x1;
    if (ry1 < y1) ry1 = y1;
    if (rx2 >= x2) rx2 = x2 - 1;
    if (ry2 >= y2) ry2 = y2 - 1;

    if (leaf){
            /* if T = int, returns number of non-zero pixels */
        if (value) return size(rx1, ry1, rx2, ry2); else return 0;
    }else if ((rx1 == x1) && (ry1 == y1) && (rx2 == x2-1) && (ry2 == y2-1)){
        return coverage;
    }

    int xq1 = (rx1 < xm) ? 0 : 1;
    int yq1 = (ry1 < ym) ? 0 : 1;
    int xq2 = (rx2 < xm) ? 0 : 1;
    int yq2 = (ry2 < ym) ? 0 : 1;

    if ((xq1 == xq2) && (yq1 == yq2)) return qt[xq1][yq1]->getcov(rx1, ry1, rx2, ry2);
    else{
        int cov = 0;
        for (int i = xq1; i <= xq2; i++) for (int j = yq1; j <= yq2; j++)
            if (qt[i][j]) cov += qt[i][j]->getcov(rx1, ry1, rx2, ry2);
        return cov;
    }
}

#endif