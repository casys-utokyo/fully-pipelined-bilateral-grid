#include<iostream>
#include<cmath>
// ignore above

// #include <ap_int.h>
// #include <hls_stream.h>
// #include <ap_axi_sdata.h>

// using namespace hls;
// typedef ap_axis<32, 1, 1, 1> AP_AXIS;
// typedef stream<AP_AXIS> AXI_STREAM;

const int height = 1080;
const int width = 1920;
const int sigma_space = 8;
// ignore const int height = 358;
// ignore const int width = 405;
// ignore const int sigma_space = 10;
const int sigma_range = 70;
// // d >= 7 and d should be odd number
const int d = 25;
const int r_space = d/2;
const float r_range = (float) (r_space * sigma_range) / sigma_space;
const int gx = ((height-1) % r_space == 0) ? round((float) (height-1)/r_space)+2 : ceil((float) (height-1)/r_space)+1;
const int gy = ((width-1) % r_space == 0) ? round((float) (width-1)/r_space)+2 : ceil((float) (width-1)/r_space)+1;
const int gz = (((int) (255 / r_range)) * r_range == 255) ? round((float) 255/r_range)+2 : ceil((float) 255/r_range)+1;

const int beg_idx = r_space - (int) ((r_space-1)/2) - 1;
const int end_idx_r = height + beg_idx - (gx-1)*r_space;
const int end_idx_c = width + beg_idx - (gy-1)*r_space;
const int lx = 3*r_space-beg_idx;

const int rem_r = r_space+end_idx_r-beg_idx-1;
const int rem_w = std::max(0, gz*(gy-2)-(width-2*r_space+beg_idx)-width);
const int loop = (height+2*r_space)*width+gx*rem_w;

const int shift_g = 10;
const int shift_gf = 4;
const int shift_s = 4;
const int shift_r = 8;

const int d1 = round((1<<shift_g) * exp((float) -r_space*r_space / (float) (2*sigma_space*sigma_space)));
const int d2 = round((1<<shift_g) * exp((float) -2*r_space*r_space / (float) (2*sigma_space*sigma_space)));
const int d3 = round((1<<shift_g) * exp((float) -3*r_space*r_space / (float) (2*sigma_space*sigma_space)));

const int imgbit = 8;
const int cidx = log2(r_space*r_space)+imgbit+1;
const int gbit = cidx + log2(r_space*r_space)+1;
const int gcbit = gbit-cidx;
const int gfbit = imgbit + shift_gf;
const int gf2bit = gfbit*2;
const int gfccidx = imgbit+(log2(r_space*r_space*9)+shift_g+1);
const int gfcbit = gfccidx*2-imgbit;
const int gzbit = gbit*gz;
const int wzbit = gbit*(gz+1);
const int gfzbit = gfbit*gz;


// typedef ap_uint<imgbit> iint;
// typedef ap_uint<gbit> gint;
// typedef ap_uint<gcbit> gcint;
// typedef ap_uint<gf2bit> gf2int;
// typedef ap_uint<gfccidx> gfgint;
// typedef ap_uint<gzbit> gzint;
// typedef ap_uint<wzbit> wzint;
// typedef ap_uint<gfzbit> gfzint;
// typedef ap_uint<gfcbit> gfcint;


// start lists: define length for lists
const int lutr = 256;
const int dif_x = r_space;
const int dif_y = r_space;
const int dif_z = 256;
const int floor_z = 256;

// ignore below
using namespace std;
#define PRINTER(name) printer(#name, (name))
#define PRINTLS(ls, len) cout << '{'; for (int iii = 0; iii < (len)-1; iii++) cout << ls[iii] << ", "; cout << ls[(len)-1] << "}\n";
template<class T>
void printer(char *name, T value) {
    cout << name << " " << value << endl;
}
int bitshift(int x, int s) {
    const int bit = (1<<(s-1));
    if (x & bit) {
        x >>= s;
        x++;
    } else {
        x >>= s;
    }
    return x;
}
int main () {
    PRINTER(r_space);
    PRINTER(r_range);
    PRINTER(gx);
    PRINTER(gy);
    PRINTER(gz);
    PRINTER(beg_idx);
    PRINTER(end_idx_r);
    PRINTER(end_idx_c);
    PRINTER(lx);
    PRINTER(rem_r);
    PRINTER(rem_w);
    PRINTER(loop);
    PRINTER(shift_g);
    PRINTER(shift_gf);
    PRINTER(shift_s);
    PRINTER(shift_r);
    PRINTER(d1);
    PRINTER(d2);
    PRINTER(d3);
    PRINTER(imgbit);
    PRINTER(cidx);
    PRINTER(gbit);
    PRINTER(gcbit);
    PRINTER(gfbit);
    PRINTER(gf2bit);
    PRINTER(gfccidx);
    PRINTER(gfcbit);
    PRINTER(gzbit);
    PRINTER(wzbit);
    PRINTER(gfzbit);

    cout << "LISTS:" << endl;

    PRINTER(lutr);
    int l_lutr[lutr];
    for (int i = 0; i < 256; i++) {
        l_lutr[i] = round((float) i / r_range);
    }
    PRINTLS(l_lutr, lutr);

    PRINTER(dif_x);
    int l_dif_x[dif_x];
    for (int i = 0; i < r_space; i++) {
        int v = bitshift((i<<(shift_s+1)) / r_space, 1);
        l_dif_x[i] = v;
    }
    PRINTLS(l_dif_x, dif_x);

    PRINTER(dif_y);
    int l_dif_y[dif_y];
    for (int i = 0; i < r_space; i++) {
        int v = bitshift((i<<(shift_s+1)) / r_space, 1);
        l_dif_y[(i+beg_idx)%r_space] = v;
    }
    PRINTLS(l_dif_y, dif_y);

    PRINTER(dif_z);
    int l_dif_z[dif_z];
    for (int i = 0; i < 256; i++) {
        l_dif_z[i] = round(((float) i / r_range - floor(i / r_range)) * (1<<shift_r));
    }
    PRINTLS(l_dif_z, dif_z);

    PRINTER(floor_z);
    int l_floor_z[floor_z];
    for (int i = 0; i < 256; i++) {
        l_floor_z[i] = i / r_range;
    }
    PRINTLS(l_floor_z, floor_z);
}