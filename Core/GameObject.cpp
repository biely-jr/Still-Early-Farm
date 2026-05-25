#include "GameObject.h"

GameObject::GameObject(Game* r_pGame, point ref, int r_width, int r_height, color fc, color bc) : Drawable (r_pGame, ref, r_width, r_height)
{
	fillColor = fc;
	borderColor = bc;
}

void GameObject::setRefPoint(point p)
{
	RefPoint = p;
}
bool GameObject::CollisionDetection(const GameObject& gObj)
{
	return RefPoint.x < gObj.RefPoint.x + gObj.width &&
		RefPoint.x + width > gObj.RefPoint.x &&
		RefPoint.y < gObj.RefPoint.y + gObj.height &&
		RefPoint.y + height > gObj.RefPoint.y;
}
