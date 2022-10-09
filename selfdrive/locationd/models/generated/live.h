#pragma once
#include "rednose/helpers/common_ekf.h"
extern "C" {
void live_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_9(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_12(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_35(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_32(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_33(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_H(double *in_vec, double *out_203406506473517518);
void live_err_fun(double *nom_x, double *delta_x, double *out_4842914450119463527);
void live_inv_err_fun(double *nom_x, double *true_x, double *out_5558287471497634478);
void live_H_mod_fun(double *state, double *out_5812090363690563332);
void live_f_fun(double *state, double dt, double *out_2226633546738004570);
void live_F_fun(double *state, double dt, double *out_4575541338218434693);
void live_h_4(double *state, double *unused, double *out_4513987903818766867);
void live_H_4(double *state, double *unused, double *out_3006032244282180376);
void live_h_9(double *state, double *unused, double *out_4497000056462252294);
void live_H_9(double *state, double *unused, double *out_1151135492072597107);
void live_h_10(double *state, double *unused, double *out_5455532297231728723);
void live_H_10(double *state, double *unused, double *out_5245411155410135023);
void live_h_12(double *state, double *unused, double *out_4655479116628509680);
void live_H_12(double *state, double *unused, double *out_3627131269329774043);
void live_h_35(double *state, double *unused, double *out_7899304017498886978);
void live_H_35(double *state, double *unused, double *out_6372694301654787752);
void live_h_32(double *state, double *unused, double *out_1305437905754740984);
void live_H_32(double *state, double *unused, double *out_8313532638987796553);
void live_h_13(double *state, double *unused, double *out_4274492553746736382);
void live_H_13(double *state, double *unused, double *out_1527371482146158470);
void live_h_14(double *state, double *unused, double *out_4497000056462252294);
void live_H_14(double *state, double *unused, double *out_1151135492072597107);
void live_h_33(double *state, double *unused, double *out_1621988405849377997);
void live_H_33(double *state, double *unused, double *out_8923492767415906260);
void live_predict(double *in_x, double *in_P, double *in_Q, double dt);
}