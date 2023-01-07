#include "bilateral_grid.h"

inline int int_interpolation(gfzint i00, gfzint i01, gfzint i10, gfzint i11, int cgx, int cgy, int z);
inline int bitshift(int x, int s);

void bg(AXI_STREAM &in_strm, AXI_STREAM &out_strm) {
#pragma HLS INTERFACE s_axilite port=return bundle=CONTROL_BUS
#pragma HLS INTERFACE axis port=in_strm
#pragma HLS INTERFACE axis port=out_strm

    stream<iint> linebuf;
    gzint grid[3][gy];
    gfzint grid_filtered[2][gy];
    gfzint line_filtered = 0;
#pragma HLS stream variable=linebuf depth=lx*width
#pragma HLS ARRAY_PARTITION variable=grid complete dim=1
#pragma HLS ARRAY_PARTITION variable=grid_filtered complete dim=1

    // initialize grid
    for (int j = 0; j < gy; j++) {
#pragma HLS PIPELINE II=1
        grid[0][j] = 0;
        grid[1][j] = 0;
    }

    AP_AXIS pix;
    gzint grid_z = 0;
    // fill grid[1][:][:] [0, r_space-beg_idx)
    // process the first group
    for (int cgx = beg_idx; cgx < r_space; cgx++) {
        for (int pgy = 0; pgy < gy; pgy++) {
            for (int cgy = 0; cgy < r_space; cgy++) {
#pragma HLS PIPELINE II=1
#pragma HLS DEPENDENCE variable=grid inter RAW false
                // load grid
                if (cgy == 0) {
                    grid_z = grid[1][pgy];
                }
                // read input, make grid, and store the input in linebuf
                const int buf_idx_c = pgy*r_space+cgy-beg_idx;
                if (buf_idx_c >= 0 && buf_idx_c < width) {
                    pix = in_strm.read();
                    const int l = pix.data;
                    int pgz = lutr[l];
                    gint gl = l;
                    pgz *= gbit;
                    gl[cidx] = 1;
                    grid_z(pgz+gbit-1,pgz) = gl + grid_z(pgz+gbit-1,pgz);
                    const int buf_idx_r = 2*r_space+cgx-beg_idx;
                    linebuf << l;
                }
                // store grid
                if (cgy == r_space-1) {
                    grid[1][pgy] = grid_z;
                }
            }
        }
    }

    // define variables for grid filtering
    int wz = gz;
    int wy = 0;
    wzint w00, w01, w02, w10, w11, w12, w20, w21, w22;
    gcint gc000, gc001, gc002, gc010, gc011, gc012, gc020, gc021, gc022, gc100, gc101, gc102, gc110, gc111, gc112, gc120, gc121, gc122, gc200, gc201, gc202, gc210, gc211, gc212, gc220, gc221, gc222;
    gfgint g000, g001, g002, g010, g011, g012, g020, g021, g022, g100, g101, g102, g110, g111, g112, g120, g121, g122, g200, g201, g202, g210, g211, g212, g220, g221, g222;
    gfcint w000, w001, w002, w010, w011, w012, w020, w021, w022, w100, w101, w102, w110, w111, w112, w120, w121, w122, w200, w201, w202, w210, w211, w212, w220, w221, w222;

    // define variables for interpolation
    gfzint i00, i01, i10, i11;

    // initialize val with the value of pix
    AP_AXIS val;
    val.keep = pix.keep;
    val.strb = pix.strb;
    val.user = pix.user;
    val.id = pix.id;
    val.dest = pix.dest;
    val.last = 0;

    // temporary buffer for grid[2][2]
    gzint grid_z2 = 0;

    // define loop variables
    int pgx = 0;
    int cgx = 0;
    int pgy = 0;
    int cgy = beg_idx;

    // main process
    // when pgx = x, read input of (x+1)-th group, perform grid filtering for x-th group,
    // and interpolate the (x-1)-th group of output
    for (int xy = 0; xy < loop; xy++) {
#pragma HLS PIPELINE II=1
#pragma HLS DEPENDENCE variable=grid inter RAW false

        // process buf_idx_c-th column in the original image
        const int buf_idx_c = pgy*r_space+cgy-beg_idx;
        // store intensity values to be read in l
        int l = -1;

        int pgz = 0;
        gint gl = 0;
        // judge whether input remains
        if (!(pgx == gx-2 && cgx >= end_idx_r) && pgx <= gx-2 && buf_idx_c < width) {
            pix = in_strm.read();
            l = pix.data;
            pgz = lutr[l];
            gl = l;
            pgz *= gbit;
            gl[cidx] = 1;
        }

        if ((cgy == 0 || buf_idx_c == 0) && cgx != 0) {
            // load grid
            grid_z = grid[2][pgy];

            grid_z(pgz+gbit-1,pgz) = gl + grid_z(pgz+gbit-1,pgz);

            // load grid for grid filtering, shift grid up by 1, and store grid
            if (wy == gy-1 && wz == gz) {
                w00 = w01;
                w01 = w02;
                w02 = 0;
                w10 = w11;
                w11 = w12;
                w12 = 0;
                w20 = w21;
                w21 = w22;
                w22 = 0;
            } else if (wy > 0 && wz == gz) {
                w00 = w01;
                w01 = w02;
                w02 = grid[0][wy+1];
                w10 = w11;
                w11 = w12;
                w12 = grid[0][wy+1] = grid[1][wy+1];
                w20 = w21;
                w21 = w22;
                if (wy == 1) {
                    w22 = grid[1][wy+1] = grid_z2;
                } else {
                    w22 = grid[1][wy+1] = grid[2][wy+1];
                }
            }
        } else {
            // grid_z should be 0 when new group starts
            if ((cgy == 0 || buf_idx_c == 0)) grid_z = 0;

            grid_z(pgz+gbit-1,pgz) = gl + grid_z(pgz+gbit-1,pgz);

            // load grid for grid filtering, shift grid up by 1, and store grid
            if (pgx < gx && cgx == r_space-1 && pgy <= 1 && cgy == r_space-1) {
                w00 = w01;
                w01 = w02;
                w02 = grid[0][pgy];
                w10 = w11;
                w11 = w12;
                w12 = grid[0][pgy] = grid[1][pgy];
                w20 = w21;
                w21 = w22;
                w22 = grid[1][pgy] = grid_z;
            } else {
                if (pgx < gx && cgx == r_space-1 && pgy == 2 && cgy == r_space-1) {
                    grid_z2 = grid_z;
                }
                if (wy == gy-1 && wz == gz) {
                    w00 = w01;
                    w01 = w02;
                    w02 = 0;
                    w10 = w11;
                    w11 = w12;
                    w12 = 0;
                    w20 = w21;
                    w21 = w22;
                    w22 = 0;
                } else if (wy > 0 && wz == gz) {
                    w00 = w01;
                    w01 = w02;
                    w02 = grid[0][wy+1];
                    w10 = w11;
                    w11 = w12;
                    w12 = grid[0][wy+1] = grid[1][wy+1];
                    w20 = w21;
                    w21 = w22;
                    if (wy == 1) {
                        w22 = grid[1][wy+1] = grid_z2;
                    } else {
                        w22 = grid[1][wy+1] = grid[2][wy+1];
                    }
                }
                // store grid
                if (pgx < gx && ((pgy != gy-1 && cgy == r_space-1) || buf_idx_c == width-1)) {
                    grid[2][pgy] = grid_z;
                }
            }
        }

        // grid filtering
        if ((pgx < gx && cgx == r_space-1 && pgy == 1 && cgy == r_space-1) || wy != 0 || wz != gz) {
            if (wz == gz) {
                // initialize gc and g
                gc001 = 0;
                gc002 = w00(gbit-1, cidx);
                gc011 = 0;
                gc012 = w01(gbit-1, cidx);
                gc021 = 0;
                gc022 = w02(gbit-1, cidx);
                gc101 = 0;
                gc102 = w10(gbit-1, cidx);
                gc111 = 0;
                gc112 = w11(gbit-1, cidx);
                gc121 = 0;
                gc122 = w12(gbit-1, cidx);
                gc201 = 0;
                gc202 = w20(gbit-1, cidx);
                gc211 = 0;
                gc212 = w21(gbit-1, cidx);
                gc221 = 0;
                gc222 = w22(gbit-1, cidx);

                g001 = 0;
                g002 = w00(cidx-1, 0);
                g011 = 0;
                g012 = w01(cidx-1, 0);
                g021 = 0;
                g022 = w02(cidx-1, 0);
                g101 = 0;
                g102 = w10(cidx-1, 0);
                g111 = 0;
                g112 = w11(cidx-1, 0);
                g121 = 0;
                g122 = w12(cidx-1, 0);
                g201 = 0;
                g202 = w20(cidx-1, 0);
                g211 = 0;
                g212 = w21(cidx-1, 0);
                g221 = 0;
                g222 = w22(cidx-1, 0);

                wz = 0;
            }

            const int idx_gs0 = gbit*(wz+1);
            const int idx_gcs0 = idx_gs0+cidx;
            const int idx_ge0 = idx_gcs0-1;
            const int idx_gce0 = idx_gs0+gbit-1;

            gc000 = gc001;
            gc001 = gc002;
            gc002 = w00(idx_gce0, idx_gcs0);
            gc010 = gc011;
            gc011 = gc012;
            gc012 = w01(idx_gce0, idx_gcs0);
            gc020 = gc021;
            gc021 = gc022;
            gc022 = w02(idx_gce0, idx_gcs0);
            gc100 = gc101;
            gc101 = gc102;
            gc102 = w10(idx_gce0, idx_gcs0);
            gc110 = gc111;
            gc111 = gc112;
            gc112 = w11(idx_gce0, idx_gcs0);
            gc120 = gc121;
            gc121 = gc122;
            gc122 = w12(idx_gce0, idx_gcs0);
            gc200 = gc201;
            gc201 = gc202;
            gc202 = w20(idx_gce0, idx_gcs0);
            gc210 = gc211;
            gc211 = gc212;
            gc212 = w21(idx_gce0, idx_gcs0);
            gc220 = gc221;
            gc221 = gc222;
            gc222 = w22(idx_gce0, idx_gcs0);

            g000 = g001;
            g001 = g002;
            g002 = w00(idx_ge0, idx_gs0);
            g010 = g011;
            g011 = g012;
            g012 = w01(idx_ge0, idx_gs0);
            g020 = g021;
            g021 = g022;
            g022 = w02(idx_ge0, idx_gs0);
            g100 = g101;
            g101 = g102;
            g102 = w10(idx_ge0, idx_gs0);
            g110 = g111;
            g111 = g112;
            g112 = w11(idx_ge0, idx_gs0);
            g120 = g121;
            g121 = g122;
            g122 = w12(idx_ge0, idx_gs0);
            g200 = g201;
            g201 = g202;
            g202 = w20(idx_ge0, idx_gs0);
            g210 = g211;
            g211 = g212;
            g212 = w21(idx_ge0, idx_gs0);
            g220 = g221;
            g221 = g222;
            g222 = w22(idx_ge0, idx_gs0);

            const gfcint w000 = (gc000, g000);
            const gfcint w001 = (gc001, g001);
            const gfcint w002 = (gc002, g002);
            const gfcint w010 = (gc010, g010);
            const gfcint w011 = (gc011, g011);
            const gfcint w012 = (gc012, g012);
            const gfcint w020 = (gc020, g020);
            const gfcint w021 = (gc021, g021);
            const gfcint w022 = (gc022, g022);
            const gfcint w100 = (gc100, g100);
            const gfcint w101 = (gc101, g101);
            const gfcint w102 = (gc102, g102);
            const gfcint w110 = (gc110, g110);
            const gfcint w111 = (gc111, g111);
            const gfcint w112 = (gc112, g112);
            const gfcint w120 = (gc120, g120);
            const gfcint w121 = (gc121, g121);
            const gfcint w122 = (gc122, g122);
            const gfcint w200 = (gc200, g200);
            const gfcint w201 = (gc201, g201);
            const gfcint w202 = (gc202, g202);
            const gfcint w210 = (gc210, g210);
            const gfcint w211 = (gc211, g211);
            const gfcint w212 = (gc212, g212);
            const gfcint w220 = (gc220, g220);
            const gfcint w221 = (gc221, g221);
            const gfcint w222 = (gc222, g222);

            const gfcint v000 = w110 + w112;
            const gfcint v001 = w101 + w121;
            const gfcint v10 = v000 + v001;

            const gfcint v003 = w000 + w002;
            const gfcint v004 = w020 + w022;
            const gfcint v11 = v003 + v004;

            const gfcint v002 = w011 + w211;
            const gfcint v20 = (v10 + v002) * d1;

            const gfcint v005 = w200 + w202;
            const gfcint v006 = w220 + w222;
            const gfcint v12 = v005 + v006;
            const gfcint v21 = (v11 + v12) * d3;

            const gfcint v30 = v20 + v21;

            const gfcint v007 = w010 + w012;
            const gfcint v008 = w001 + w021;
            const gfcint v009 = w100 + w102;
            const gfcint v010 = w120 + w122;
            const gfcint v011 = w210 + w212;
            const gfcint v012 = w201 + w221;
            const gfcint v13 = v007 + v008;
            const gfcint v14 = v009 + v010;
            const gfcint v15 = v011 + v012;
            const gfcint v22 = v13 + v14;
            const gfcint v31 = (v22 + v15) * d2;

            const gfcint v013 = w111 << shift_g;

            const gfcint v40 = v30 + v31 + v013;

            const int idxf_s = gfbit*wz;
            const int idxf_e = idxf_s+gfbit-1;

            const long long numerator = v40(gfccidx-1, 0);
            const int denominator = v40(gfcbit-1, gfccidx);

            line_filtered(idxf_e,idxf_s) = (denominator == 0) ? 0 : bitshift((numerator<<(shift_gf+1)) / denominator, 1);
            // store values in grid_filtered after processing each column
            if (wz == gz-1) {
                grid_filtered[1][wy] = line_filtered;
                wy = (wy == gy-1) ? 0 : wy+1;
            }
            wz++;
        }

        // shift grid_filtered up, interpolate grid, and store output
        if (cgy >= 0 && buf_idx_c < width) {
            if (pgx == 1 && cgx == r_space-1) {
                if (buf_idx_c <= 1) {
                    grid_filtered[0][buf_idx_c] = grid_filtered[1][buf_idx_c];
                } else if (cgy == beg_idx) {
                    grid_filtered[0][pgy+1] = grid_filtered[1][pgy+1];
                }
            } else if (pgx >= 2) {
                if (buf_idx_c == 0) {
                    i00 = grid_filtered[0][0];
                    if (cgx == r_space-1) {
                        i10 = grid_filtered[0][0] = grid_filtered[1][0];
                    } else {
                        i10 = grid_filtered[1][0];
                    }
                } else if (buf_idx_c == 1) {
                    i01 = grid_filtered[0][1];
                    if (cgx == r_space-1) {
                        i11 = grid_filtered[0][1] = grid_filtered[1][1];
                    } else {
                        i11 = grid_filtered[1][1];
                    }
                } else if (cgy == beg_idx) {
                    i00 = i01;
                    i01 = grid_filtered[0][pgy+1];
                    i10 = i11;
                    if (cgx == r_space-1) {
                        i11 = grid_filtered[0][pgy+1] = grid_filtered[1][pgy+1];
                    } else {
                        i11 = grid_filtered[1][pgy+1];
                    }
                }
                val.data = int_interpolation(i00, i01, i10, i11, cgx, cgy, linebuf.read());
                // last should be 1 when it is the last pixel
                if (pgx == gx && cgx == rem_r && buf_idx_c == width-1) val.last = 1;
                out_strm << val;
            }

            if (l >= 0) linebuf << l;
        }

        // update loop variables
        if (cgx == r_space-1 && pgy == gy-1) {
            if (cgy == end_idx_c+rem_w-1) {
                cgy = beg_idx;
                pgy = 0;
                cgx = 0;
                pgx++;
            } else {
                cgy++;
            }
        } else {
            if (buf_idx_c == width-1) {
                cgy = beg_idx;
                pgy = 0;
                cgx++;
            } else if (cgy == r_space-1) {
                cgy = 0;
                pgy++;
            } else {
                cgy++;
            }
        }
    }
}

inline int int_interpolation(gfzint i00, gfzint i01, gfzint i10, gfzint i11, int cgx, int cgy, int z) {
    const int x0 = dif_x[cgx];
    const int y0 = dif_y[cgy];
    const int z0 = dif_z[z];
    const int z1 = (1<<shift_r)-z0;
    const int x0y0 = x0*y0;
    const int x0y1 = (x0<<shift_s)-x0y0;
    const int x1y0 = (y0<<shift_s)-x0y0;
    const int x1y1 = (1<<(shift_s*2))-(x0<<shift_s)-x1y0;
    const int iz0 = floor_z[z];
    const int vl = iz0*gfbit;
    const int vh = vl+2*gfbit-1;
    const gf2int i00c = i00(vh, vl);
    const gf2int i01c = i01(vh, vl);
    const gf2int i10c = i10(vh, vl);
    const gf2int i11c = i11(vh, vl);
    const int v00 = i00c(gfbit-1, 0) * z1 + i00c(gf2bit-1, gfbit) * z0;
    const int v01 = i10c(gfbit-1, 0) * z1 + i10c(gf2bit-1, gfbit) * z0;
    const int v10 = v00 * x1y1 + v01 * x0y1;

    const int v02 = i01c(gfbit-1, 0) * z1 + i01c(gf2bit-1, gfbit) * z0;
    const int v03 = i11c(gfbit-1, 0) * z1 + i11c(gf2bit-1, gfbit) * z0;
    const int v11 = v02 * x1y0 + v03 * x0y0;

    return bitshift((v10 + v11), shift_gf+(shift_s*2)+shift_r);
}

inline int bitshift(int x, int s) {
    const int bit = (1<<(s-1));
    if (x & bit) {
        x >>= s;
        x++;
    } else {
        x >>= s;
    }
    return x;
}