/*
 * Copyright (C) Robert Lainé & Jeremy Lainé
 * See AUTHORS file for a full list of contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SAILWRITER_HAND_H
#define SAILWRITER_HAND_H

#include "sailwriter-txt.h"

/** This class allows you to write a developed sail by using
 *  the deviation of the edge from the straight line instead
 *  of using absolute coordinates to a TXT file.
 *
 * @ingroup FileIo
 */
class CSailHandWriter : public CSailTxtWriter
{
public:
    virtual void write(const CPanelGroup &sail, const QString &filename) const;
};


#endif
