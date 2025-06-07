//
// Created by silly on 23.05.25.
//

#include "GameView.h"

GameView::GameView() : MDISubWindow(nullptr, "Game", QRect(20, 40, 1450, 800))
{
    centralWidget = new QWidget(this);
    layout()->addWidget(centralWidget);

    view = new QGraphicsView();
    view->scale(0.3, 0.3);

    zoomInButton = new QAction();

    zoomOutButton = new QAction();

    toolBar = new QToolBar();
    toolBar->setMovable(false);

    viewLayout = new QGridLayout(centralWidget);
    viewLayout->addWidget(toolBar, 0, 0);
    viewLayout->addWidget(view, 1, 0);

    zoomInButton->setIcon(QIcon(":/icons/zoomIn"));
    zoomInButton->setToolTip("Zoom In");

    zoomOutButton->setIcon(QIcon(":/icons/zoomOut"));
    zoomOutButton->setToolTip("Zoom Out");

    connect(zoomInButton, &QAction::triggered, this, &GameView::zoomIn);
    connect(zoomOutButton, &QAction::triggered, this, &GameView::zoomOut);

    scrollVBar = new QScrollBar();
    scrollVBar->setValue(0);
    scrollHBar = new QScrollBar();
    scrollHBar->setValue(0);

    view->setVerticalScrollBar(scrollVBar);
    view->setHorizontalScrollBar(scrollHBar);

    spacer = new QWidget();
}

void GameView::setGame(Game *game)
{
    this->game = game;
    view->setScene(game->scene);
    connect(game, &Game::playerHealthChanged, this, [this]() {
        updateToolbarIcons();
    });
    updateToolbarIcons();

    scrollVBar->setValue(0);
    scrollHBar->setValue(0);

    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void GameView::updateGame()
{
    view->update();
    viewLayout->update();
    MDISubWindow::update();
    this->update();
}

void GameView::execute()
{
    view->setScene(game->scene);

    scrollVBar->setValue(0);
    scrollHBar->setValue(0);
}


void GameView::updateToolbarIcons()
{
    toolBar->clear();
    hearts.clear();
    toolBar->addAction(zoomInButton);
    toolBar->addAction(zoomOutButton);
    toolBar->addWidget(spacer);

    for (int i = 1; i <= game->getPlayerHP(); ++i) {
        hearts.append(new QAction(heartIcon, nullptr));
        toolBar->addAction(hearts[i-1]);
    }

    toolBar->update();
}

