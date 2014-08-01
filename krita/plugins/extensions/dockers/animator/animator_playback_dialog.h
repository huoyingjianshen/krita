/*
 *  Copyright (c) 2014 Somsubhra Bairi <somsubhra.bairi@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License, or(at you option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef ANIMATOR_PLAYBACK_DIALOG_H
#define ANIMATOR_PLAYBACK_DIALOG_H

#include <QDialog>

#include "kis_animation.h"

/**
 * The animator playback options dialog
 */
class AnimatorPlaybackDialog : public QDialog
{
    Q_OBJECT
public:
    AnimatorPlaybackDialog(QWidget* parent=0);
    void setModel(KisAnimation* model);

private slots:
    void enableLooping(bool enable);
    void setFps(int value);
    void setLocalPlaybackRange(int value);

private:
    KisAnimation* m_model;

signals:
    void playbackStateChanged();
};

#endif // ANIMATOR_PLAYBACK_DIALOG_H
