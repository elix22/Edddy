/* Edddy
// Copyright (C) 2017 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "editmaster.h"
#include "inputmaster.h"
#include "edddycursor.h"

#include "tool.h"

HashMap<StringHash, SharedPtr<Tool>> Tool::tools_{};

Tool::Tool(Context* context) : Object(context)
{
}

void Tool::UpdatePreview(bool shiftDown, bool ctrlDown, bool altDown)
{
    GetCursor()->RemoveAllInstanceNodes();
}

void Tool::Init()
{
    if (!tools_.Contains(GetType()))
        tools_[GetType()] = SharedPtr<Tool>(this);
}

bool Tool::IsLastTool()
{
    return GetType() == GetSubsystem<EditMaster>()->GetLastToolType();
}

EdddyCursor* Tool::GetCursor()
{
    return GetSubsystem<InputMaster>()->GetCursor();
}

const HashSet<IntVector3> Tool::BresenhamLine(const IntVector3 start, const IntVector3 end)
{
    HashSet<IntVector3> line{};
    int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
    IntVector3 point{ start };

    dx = end.x_ - start.x_;
    dy = end.y_ - start.y_;
    dz = end.z_ - start.z_;
    x_inc = (dx < 0) ? -1 : 1;
    l = abs(dx);
    y_inc = (dy < 0) ? -1 : 1;
    m = abs(dy);
    z_inc = (dz < 0) ? -1 : 1;
    n = abs(dz);
    dx2 = l << 1;
    dy2 = m << 1;
    dz2 = n << 1;

    if ((l >= m) && (l >= n)) {
        err_1 = dy2 - l;
        err_2 = dz2 - l;
        for (i = 0; i < l; i++) {
            line.Insert(point);
            if (err_1 > 0) {
                point.y_ += y_inc;
                err_1 -= dx2;
            }
            if (err_2 > 0) {
                point.z_ += z_inc;
                err_2 -= dx2;
            }
            err_1 += dy2;
            err_2 += dz2;
            point.x_ += x_inc;
        }
    } else if ((m >= l) && (m >= n)) {
        err_1 = dx2 - m;
        err_2 = dz2 - m;
        for (i = 0; i < m; i++) {
            line.Insert(point);
            if (err_1 > 0) {
                point.x_ += x_inc;
                err_1 -= dy2;
            }
            if (err_2 > 0) {
                point.z_ += z_inc;
                err_2 -= dy2;
            }
            err_1 += dx2;
            err_2 += dz2;
            point.y_ += y_inc;
        }
    } else {
        err_1 = dy2 - n;
        err_2 = dx2 - n;
        for (i = 0; i < n; i++) {
            line.Insert(point);
            if (err_1 > 0) {
                point.y_ += y_inc;
                err_1 -= dz2;
            }
            if (err_2 > 0) {
                point.x_ += x_inc;
                err_2 -= dz2;
            }
            err_1 += dy2;
            err_2 += dx2;
            point.z_ += z_inc;
        }
    }
    line.Insert(point);

    return line;
}
