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
void car_err_fun(double *nom_x, double *delta_x, double *out_2123858726471964323);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_147094582364114116);
void car_H_mod_fun(double *state, double *out_4046455293741089611);
void car_f_fun(double *state, double dt, double *out_2561792606746991469);
void car_F_fun(double *state, double dt, double *out_6749487635925460135);
void car_h_25(double *state, double *unused, double *out_4036485789383440967);
void car_H_25(double *state, double *unused, double *out_5024108742981173493);
void car_h_24(double *state, double *unused, double *out_2369282182506852092);
void car_H_24(double *state, double *unused, double *out_4194570144659182898);
void car_h_30(double *state, double *unused, double *out_5272540999642259467);
void car_H_30(double *state, double *unused, double *out_496412412853565295);
void car_h_26(double *state, double *unused, double *out_8436057024560478219);
void car_H_26(double *state, double *unused, double *out_1282605424107117269);
void car_h_27(double *state, double *unused, double *out_2005415766435312201);
void car_H_27(double *state, double *unused, double *out_2720006484037508512);
void car_h_29(double *state, double *unused, double *out_7531473202986335314);
void car_H_29(double *state, double *unused, double *out_1006643757167957479);
void car_h_28(double *state, double *unused, double *out_8035671487630012762);
void car_H_28(double *state, double *unused, double *out_4075755259901573095);
void car_h_31(double *state, double *unused, double *out_5556457641467274679);
void car_H_31(double *state, double *unused, double *out_656397321873765793);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}