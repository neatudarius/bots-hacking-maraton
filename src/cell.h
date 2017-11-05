#include 
typedef std::pair<int, int> pair;

struct cell {
    // index from 9x9 matrix
    int lin_big, col_big;
   
    // index from LOCAL 3x3 matrix
    int lin_small, col_small, diag_small;

    // index from macroboard
    int lin_macro, col_macro, diag_macro;

    // number of current 3x3 square
    int cell_index;

    /**  compute informations for a cell (i,j) from big 9x9 matrix **/
    cell(int i, int j);

    /** check if I can move in this cell **/
    bool canBePicked(const int macroboard[][3], const int field[][9]);

    /** for debug - cell info **/
    string toString() const;

    pair getBig() const;


    pair getSmall() const ;


    pair getMacro() const;
};



pair cell::getBig() const {
    return pair(lin_big, col_big);
}

pair cell::getSmall() const {
    return pair(lin_small, col_small);
}


pair cell::getMacro() const {
    return pair(lin_macro, col_macro);
}

/**  compute informations for a cell (i,j) from big 9x9 matrix **/
cell::cell(int i, int j) {
    // field - matrix 9x9
    lin_big = i;
    col_big = j;

    // small matrix 3x3
    cell_index = 3 * (i / 3) + (j / 3);
    lin_small = lin_big % 3;
    col_small = col_big % 3;
    if (lin_small == col_small && lin_small + col_small == 2)
        diag_small = 3;
    else if (lin_small == col_small)
        diag_small = 1;
    else if (lin_small + col_small == 2)
        diag_small = 2;
    else diag_small = 0;
    // diag_small = (lin_small == col_small ?      1 : // principal
    //              ((lin_small + col_small == 2) ?  2 : // secundary
    //                                             0)); // none
    //                                          // always check if (diag_small && exp(diag_small))
    //                                          // before use diag_small in expression

    // macroboard - matrix 3x3
    lin_macro = cell_index / 3;
    col_macro= cell_index % 3;
    if (lin_macro == col_macro && lin_macro + col_macro == 2)
        diag_macro = 3;
    else if (lin_macro == col_macro)
        diag_macro = 1;
    else if (lin_macro + col_macro == 2)
        diag_macro = 2;
    else diag_macro = 0;
    // diag_macro = (lin_macro == col_macro ?      1 : // principal
    //              ((lin_macro + col_macro == 2) ?  2 : // secundary
    //                                             0)); // none
    //                                          // always check if (diag_macro && exp(diag_macro))
    //                                          // before use diag_macro in expression
}

/** check if I can move in this cell **/
bool cell::canBePicked(const int macroboard[][3], const int field[][9]) {
    return macroboard[ lin_macro ][ col_macro ] == -1
           && !field[ lin_big ][ col_big ];
}

/** for debug - cell info **/
string cell::toString() const {
    string big = "Big   : ";
    big += ( (char) '0' + lin_big );
    big += ",";
    big += ( (char) '0' + col_big );
    big += "\n";


    string small = "Small : ";
    small += ( (char) '0' + lin_small );
    small += ",";
    small += ( (char) '0' + col_small );
    small += " in square #";
    small += ( (char) '0' + cell_index );
    small +=  "\n";


    string macro = "Macro : ";
    macro += ( (char) '0' + lin_macro );
    macro += ",";
    macro += ( (char) '0' + col_macro );
    macro += "\n";

    return big + small + macro;
}
