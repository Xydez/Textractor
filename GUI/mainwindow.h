#pragma once

#include "types.h"
#include "qtcommon.h"

class MainWindow : public QMainWindow
{
public:
	explicit MainWindow(SettingsPack settingsPack, QWidget *parent = nullptr);
	~MainWindow();
private:
	void closeEvent(QCloseEvent*);

	SettingsPack m_settingsPack;
};
