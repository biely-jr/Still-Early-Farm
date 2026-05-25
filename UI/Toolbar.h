#pragma once
#include "../Core/Drawable.h"

class ToolbarIcon :public Drawable
{
private:
	string image_path;
public:
	ToolbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void onClick() = 0;
};

class RestartIcon : public ToolbarIcon
{
public:
	RestartIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
};

class ExitIcon : public ToolbarIcon
{
public:
	ExitIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
};

class PauseIcon : public ToolbarIcon
{
public:
	PauseIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void onClick();
};

class ResumeIcon : public ToolbarIcon
{
public:
	ResumeIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
};

class SaveIcon : public ToolbarIcon
{
public:
	SaveIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
};

class LoadGameIcon : public ToolbarIcon
{
public:
	LoadGameIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
};

enum ICONS
{
	ICON_RESTART,
	ICON_PAUSE,
	ICON_RESUME,
	ICON_SAVE,
	ICON_LOAD,
	ICON_EXIT,
	ICON_COUNT
};

class Toolbar : public Drawable
{
private:
	ToolbarIcon** iconsList;
	string iconsImages[ICON_COUNT];

public:
	Toolbar(Game* r_pGame, point r_point, int r_width, int r_height);
	~Toolbar();
	void draw() const override;
	bool handleClick(int x, int y);
};
