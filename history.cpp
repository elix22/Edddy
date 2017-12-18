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

#include "blockinstance.h"
#include "gridblock.h"
#include "freeblock.h"

#include "history.h"

History::History(Context* context) : Object(context),
    steps_{},
    stepIndex_{0},
    currentStep_{}
{
}

void History::EndStep()
{
    //Don't write history if nothing happened
    bool anyChange{ false };
    for (const Change& change : currentStep_) {
        if (change.Any())
            anyChange = true;
    }
    if (!anyChange) {
        currentStep_.Clear();
        return;
    }

    //Clear future
    while (steps_.Size() > stepIndex_) {

        steps_.Pop();
    }
    steps_.Push(currentStep_);
    currentStep_.Clear();

    ++stepIndex_;
}

void History::Undo()
{
    if (stepIndex_ > 0) {

        --stepIndex_;
        for (Change c : steps_.At(stepIndex_)) {

            GridBlock* gridBlock{ static_cast<GridBlock*>(c.instance_) };

            if (!gridBlock) {

                if (c.block_.first_ == nullptr) {

                    c.instance_->GetNode()->Remove();

                } else if (c.block_.second_ == nullptr) {
                    ///Restore free block

                }
            } else {

                c.instance_->SetBlock(c.block_.first_, c.rotation_.first_);
            }
        }
    }
}
void History::Redo()
{
    if (steps_.Size() - stepIndex_ > 0) {

        for (Change c : steps_.At(stepIndex_)) {

            GridBlock* gridBlock{ static_cast<GridBlock*>(c.instance_) };

            if (!gridBlock) {

                if (c.block_.first_ == nullptr) {
                    ///Readd free block

                } else if (c.block_.second_ == nullptr) {

                    c.instance_->GetNode()->Remove();
                }
            } else {

                c.instance_->SetBlock(c.block_.second_, c.rotation_.second_);
            }
        }
        ++stepIndex_;
    }
}
