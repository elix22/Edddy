/* Edddy
// Copyright (C) 2016 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// Commercial licenses are available through frode@lindeijer.nl
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

#ifndef GRIDBLOCK_H
#define GRIDBLOCK_H

#include <Urho3D/Urho3D.h>

#include "blockinstance.h"

class GridBlock : public BlockInstance
{
    friend class BlockMap;

    URHO3D_OBJECT(GridBlock, BlockInstance);
public:
    GridBlock(Context* context);
    static void RegisterObject(Context* context);
    virtual void OnNodeSet(Node* node);

    virtual void Update(float timeStep);
    void HandleCursorStep(StringHash eventType, VariantMap& evenData);
    virtual void DelayedStart();
    bool WithinLock();
    IntVector3 GetCoords() const { return coords_; }
private:
    static StaticModelGroup* activeCenterGroup_;
    static StaticModelGroup* inactiveCenterGroup_;
    static StaticModelGroup* sideGroup_;

    IntVector3 coords_;

    Node* sidesNode_;
    Node* centerNode_;
    Vector<Node*> sideNodes_;
    StaticModel* centerModel_;
    //Called by BlockMap
    void Init(IntVector3 coords);
    void SetCoords(IntVector3 coords);
    void UpdatePosition();
    void CreateSideNodes();
};

#endif // GRIDBLOCK_H
