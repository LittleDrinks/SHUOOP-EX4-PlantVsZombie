#pragma once

class Controller;

// PVZ2-like global controller pointer (compat layer)
extern Controller* MainController;

// Request to quit the game loop (e.g. window close).
extern bool QuitRequested;
