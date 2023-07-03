#pragma once
#include "rednose/helpers/common_ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_8389671773090003272);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8397899229625241474);
void car_H_mod_fun(double *state, double *out_756356007682516085);
void car_f_fun(double *state, double dt, double *out_5832708999682159094);
void car_F_fun(double *state, double dt, double *out_107648160780602868);
void car_h_25(double *state, double *unused, double *out_8728890367691471987);
void car_H_25(double *state, double *unused, double *out_4299801806967638734);
void car_h_24(double *state, double *unused, double *out_1106583115750441803);
void car_H_24(double *state, double *unused, double *out_446707519998870300);
void car_h_30(double *state, double *unused, double *out_7626165709969085272);
void car_H_30(double *state, double *unused, double *out_7230251925250296127);
void car_h_26(double *state, double *unused, double *out_7022849825292607755);
void car_H_26(double *state, double *unused, double *out_558298488093582510);
void car_h_27(double *state, double *unused, double *out_1566067537315907542);
void car_H_27(double *state, double *unused, double *out_9041728836658830578);
void car_h_29(double *state, double *unused, double *out_6956261716858252833);
void car_H_29(double *state, double *unused, double *out_6720020580935903943);
void car_h_28(double *state, double *unused, double *out_6907934773201938105);
void car_H_28(double *state, double *unused, double *out_2245967092719748971);
void car_h_31(double *state, double *unused, double *out_1195524451843919254);
void car_H_31(double *state, double *unused, double *out_4330447768844599162);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}