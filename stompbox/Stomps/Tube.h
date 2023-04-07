#pragma once
/*
 * Copyright (C) 2012 Hermann Meyer, Andreas Degert, Pete Shorthose, Steve Poskitt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */


#pragma once

#ifndef SRC_HEADERS_VALVE_H_
#define SRC_HEADERS_VALVE_H_

#define __rt_data
#include <cstdint>


 /****************************************************************
  * 1-dimensional function tables for linear interpolation
  *
  * table1d and table1d_imp<size> must only differ in the last
  * element, so that the typecast for tubetab below will work.
  * Can't use inheritance because then C initializers will not
  * work and initialization will be more awkward or less efficient.
  */

struct table1d   // 1-dimensional function table
{
    float low;
    float high;
    float istep;
    int32_t size;
    float data[];
};

template <int32_t tab_size>
struct table1d_imp
{
    float low;
    float high;
    float istep;
    int32_t size;
    float data[tab_size];
    operator table1d& () const
    {
        return *(table1d*)this;
    }
};


enum {
    TUBE_TABLE_12AX7_68k,
    TUBE_TABLE_12AX7_250k,
    TUBE_TABLE_6V6_68k,
    TUBE_TABLE_6V6_250k,
    TUBE_TABLE_12AU7_68k,
    TUBE_TABLE_12AU7_250k,
    TUBE_TABLE_6DJ8_68k,
    TUBE_TABLE_6DJ8_250k,
    TUBE_TABLE_12AT7_68k,
    TUBE_TABLE_12AT7_250k,
    TUBE_TABLE_6C16_68k,
    TUBE_TABLE_6C16_250k,
    TUBE_TABLE_6L6CG_68k,
    TUBE_TABLE_6L6CG_250k,
    TUBE_TABLE_EL34_68k,
    TUBE_TABLE_EL34_250k,
    TUBE_TABLE_12AY7_68k,
    TUBE_TABLE_12AY7_250k,
    TUBE_TABLE_JJECC83S_68k,
    TUBE_TABLE_JJECC83S_250k,
    TUBE_TABLE_JJECC99_68k,
    TUBE_TABLE_JJECC99_250k,
    TUBE_TABLE_EL84_68k,
    TUBE_TABLE_EL84_250k,
    TUBE_TABLE_EF86_68k,
    TUBE_TABLE_EF86_250k,
    TUBE_TABLE_SVEL34_68k,
    TUBE_TABLE_SVEL34_250k,
    TUBE_TABLE_SIZE
};

double Ftube(int32_t table, double Vgk);
double VoltageScale(int32_t table);

#endif
