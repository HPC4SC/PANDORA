/*
 * Copyright (c) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACI-N
 * http://www.bsc.es
 *
 * This file is part of Cassandra.
 * Cassandra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Cassandra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef __LoadingProgressBar_hxx__
#define __LoadingProgressBar_hxx__

#include <ui_LoadingProgressBar.h>

namespace GUI
{

class LoadingProgressBar : public QWidget
{
	Q_OBJECT

	Ui::LoadingProgressBar _loadingProgressBar;
public:
	LoadingProgressBar();
	virtual ~LoadingProgressBar();
	void initLoading();

public slots:
	void updateProgress();
};

} // namespace GUI

#endif // __LoadingProgressBar_hxx__

