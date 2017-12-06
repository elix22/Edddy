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

#ifndef HISTORY_H
#define HISTORY_H

#include <Urho3D/Urho3D.h>
#include "luckey.h"

class Block;
class BlockInstance;

struct Change {

    BlockInstance* instance_;

    Pair<Vector3, Vector3> position_;
    Pair<Quaternion, Quaternion> rotation_;
    Pair<Block*, Block*> block_;

    bool Any() { return !(block_.first_ == nullptr && block_.second_ == nullptr) &&
                        !(  position_.first_ == position_.second_
                         && rotation_.first_ == rotation_.second_
                         && block_.first_ == block_.second_); }
};

typedef Vector<Change> Step;

class History : public Object
{
    URHO3D_OBJECT(History, Object);
public:
    History(Context* context);

    void AddChange(Change change) { currentStep_.Push(change); }
    void EndStep();
    void Undo();
    void Redo();
private:
    Vector<Step> steps_;
    unsigned stepIndex_;
    Step currentStep_;
};

#endif // HISTORY_H
