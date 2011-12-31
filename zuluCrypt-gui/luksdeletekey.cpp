/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "luksdeletekey.h"
#include "zulucrypt.h"
#include "../zuluCrypt-cli/executables.h"
#include "miscfunctions.h"

#include <QObject>
#include <Qt>
#include <QObject>
#include <QFileDialog>
#include <QRadioButton>
#include <QPushButton>
#include <QMessageBox>
#include <QCloseEvent>
#include <QIcon>
#include <QKeyEvent>

namespace Ui {
    class luksdeletekey;
}

luksdeletekey::luksdeletekey(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::luksdeletekey)
{
	m_ui->setupUi(this);
	this->setFixedSize(this->size());

	m_openPartition = new openpartition(this);
	m_openPartition->setWindowFlags(Qt::Window | Qt::Dialog);

	m_ldk = NULL ;
	connect(m_openPartition,
		SIGNAL(clickedPartition(QString)),
		this,
		SLOT(deleteKey(QString)));
	connect(this,
		SIGNAL(pbOpenPartitionClicked()),
		m_openPartition,
		SLOT(ShowUI()));
	connect(m_ui->pushButtonDelete,
		SIGNAL(clicked()),
		this,
		SLOT(pbDelete())) ;
	connect(m_ui->pushButtonCancel,
		SIGNAL(clicked()),
		this,
		SLOT(pbCancel())) ;
	connect(m_ui->rbPassphrase,
		SIGNAL(toggled(bool)),
		this,
		SLOT(rbPassphrase())) ;
	connect(m_ui->rbPassphraseFromFile,
		SIGNAL(toggled(bool)),
		this,
		SLOT(rbPassphraseFromFile())) ;
	connect(m_ui->pushButtonOpenKeyFile,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenKeyFile())) ;
	connect(m_ui->pushButtonOpenVolume,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenVolume()));
	connect(m_ui->pushButtonOpenPartition,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenPartition())) ;
}

void luksdeletekey::closeEvent(QCloseEvent *e)
{
	e->ignore();
	if( m_ldk == NULL )
		pbCancel();
}

void luksdeletekey::rbPassphrase()
{
	m_ui->labelPassphrase->setText(tr("passphrase"));
	m_ui->lineEditPassphrase->setEchoMode(QLineEdit::Password);
	m_ui->lineEditPassphrase->clear();
	m_ui->pushButtonOpenKeyFile->setEnabled(false);
	m_ui->pushButtonOpenKeyFile->setIcon(QIcon(QString(":/passphrase.png")));
}

void luksdeletekey::rbPassphraseFromFile()
{
	m_ui->labelPassphrase->setText(tr("keyfile"));
	m_ui->lineEditPassphrase->setEchoMode(QLineEdit::Normal);
	m_ui->lineEditPassphrase->clear();
	m_ui->pushButtonOpenKeyFile->setEnabled(true);
	m_ui->pushButtonOpenKeyFile->setIcon(QIcon(QString(":/keyfile.png")));
}

void luksdeletekey::pbOpenKeyFile()
{
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("key file with a passphrase to delete"),
						 QDir::homePath(),
						 0);
	m_ui->lineEditPassphrase->setText( Z );
}

void luksdeletekey::ShowUI()
{
	enableAll();
	m_ui->rbPassphrase->setEnabled(true);
	m_ui->labelPassphrase->setText(tr("passphrase"));
	m_ui->rbPassphrase->setChecked(true);
	m_ui->lineEditVolumePath->setFocus();
	m_ui->pushButtonCancel->setDefault(true);
	m_ui->pushButtonOpenPartition->setIcon(QIcon(QString(":/partition.png")));
	m_ui->pushButtonOpenVolume->setIcon(QIcon(QString(":/file.png")));
	this->show();
}

void luksdeletekey::disableAll()
{
	m_ui->groupBox->setEnabled(false);
	m_ui->label->setEnabled(false);
	m_ui->labelPassphrase->setEnabled(false);
	m_ui->lineEditPassphrase->setEnabled(false);
	m_ui->lineEditVolumePath->setEnabled(false);
	m_ui->pushButtonCancel->setEnabled(false);
	m_ui->pushButtonDelete->setEnabled(false);
	m_ui->pushButtonOpenKeyFile->setEnabled(false);
	m_ui->pushButtonOpenPartition->setEnabled(false);
	m_ui->pushButtonOpenVolume->setEnabled(false);
	m_ui->rbPassphrase->setEnabled(false);
	m_ui->rbPassphraseFromFile->setEnabled(false);
}

void luksdeletekey::enableAll()
{
	m_ui->groupBox->setEnabled(true);
	m_ui->label->setEnabled(true);
	m_ui->labelPassphrase->setEnabled(true);
	m_ui->lineEditPassphrase->setEnabled(true);
	m_ui->lineEditVolumePath->setEnabled(true);
	m_ui->pushButtonCancel->setEnabled(true);
	m_ui->pushButtonDelete->setEnabled(true);
	m_ui->pushButtonOpenKeyFile->setEnabled(true);
	m_ui->pushButtonOpenPartition->setEnabled(true);
	m_ui->pushButtonOpenVolume->setEnabled(true);
	m_ui->rbPassphrase->setEnabled(true);
	m_ui->rbPassphraseFromFile->setEnabled(true);
}

void luksdeletekey::pbCancel()
{
	HideUI() ;
}

void luksdeletekey::pbOpenPartition()
{
	emit pbOpenPartitionClicked();
}
void luksdeletekey::UIMessage(QString title, QString message)
{
	QMessageBox m ;
	m.setFont(this->font());
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setText(message);
	m.setWindowTitle(title);
	m.addButton(QMessageBox::Ok);
	m.exec() ;
}
void luksdeletekey::pbDelete()
{
	m_volumePath = m_ui->lineEditVolumePath->text() ;

	QString passphrase = m_ui->lineEditPassphrase->text() ;

	if ( passphrase == QString("") ){
		UIMessage(tr("ERROR"),tr("the passphrase field is empty"));
		return ;
	}
	if( m_volumePath.mid(0,2) == QString("~/"))
		m_volumePath = QDir::homePath() + QString("/") + m_volumePath.mid(2) ;

	if ( m_volumePath == QString("") ){
		UIMessage(tr("ERROR"),tr("the path to encrypted volume field is empty"));
		return ;
	}
	if( m_volumePath.mid(0,5) == QString("UUID=")){
		if( miscfunctions::isUUIDvalid(m_volumePath) == false ){
			UIMessage(tr("ERROR"),tr("could not find any partition with the presented UUID"));
			return ;
		}
	}
	if(miscfunctions::exists(m_volumePath) == false && m_volumePath.mid(0,5) != QString("UUID=")){
		UIMessage(tr("ERROR"),tr("volume path field does not point to a file or device"));
		return ;
	}
	if(m_ui->rbPassphraseFromFile->isChecked() == true){
		if( passphrase.mid(0,2) == QString("~/"))
			passphrase = QDir::homePath() + QString("/") + passphrase.mid(2);

		if(QFile::exists(passphrase) == false){
			UIMessage(tr("ERROR"),tr("key file field does not point to a file"));
			return ;
		}
	}
	m_volumePath.replace("\"","\"\"\"") ;

	if ( miscfunctions::isLuks(m_volumePath) == false ){
		UIMessage(tr("ERROR"),tr("given path does not point to a luks volume"));
		return ;
	}
	if(miscfunctions::luksEmptySlots(m_volumePath).at(0) == QString("1")){
		QString s = tr("There is only one last key in the volume.");
		s = s + tr("\nDeleting it will make the volume unopenable and lost forever.") ;
		s = s + tr("\nAre you sure you want to delete this key?");

		QMessageBox m ;
		m.setFont(this->font());
		m.setParent(this);
		m.setWindowFlags(Qt::Window | Qt::Dialog);
		m.setWindowTitle(tr("WARNING!"));
		m.setText(s) ;
		m.addButton(QMessageBox::Yes);
		m.addButton(QMessageBox::No);
		m.setDefaultButton(QMessageBox::No);
		if( m.exec() == QMessageBox::No )
			return ;
	}	
	QString exe = QString(ZULUCRYPTzuluCrypt) ;
	exe = exe + QString(" removekey ")  ;
	exe = exe + QString("\"") +  m_volumePath + QString("\"") ;

	if ( m_ui->rbPassphraseFromFile->isChecked() == true )
		exe = exe + QString(" -f ") ;
	else
		exe = exe + QString(" -p ") ;

	passphrase.replace("\"","\"\"\"") ;

	exe = exe + QString(" \"") + passphrase + QString("\"") ;

	disableAll();

	m_ldk = new runInThread(exe) ;
	connect(m_ldk,
		SIGNAL(finished(runInThread *,int)),
		this,
		SLOT(threadfinished(runInThread *,int))) ;
	m_ldk->start();
}

void luksdeletekey::threadfinished(runInThread * ldk,int status)
{
	ldk->deleteLater(); ;
	ldk = NULL ;
	QStringList l ;
	QString success;
	switch( status ){
		case 0 :
			l = miscfunctions::luksEmptySlots(m_volumePath) ;
			success = tr("key removed successfully.\n%1 / %2 slots are now in use").arg(l.at(0)).arg(l.at(1));
			UIMessage(tr("SUCCESS"),success);
			HideUI() ;
			return ;
		case 1 :UIMessage(tr("ERROR"),tr("device is not a luks device or does not exist"));
			break ;
		case 2 :UIMessage(tr("ERROR"),tr("there is no key in the volume that match entered key"));
			break ;
		case 3 :UIMessage(tr("ERROR"),tr("could not open luks device"));
			break ;
		case 7 :UIMessage(tr("ERROR"),tr("could not get enough memory to open the key file"));
			break ;
		case 11 :UIMessage(tr("ERROR"),
			tr("presented UUID does not match any UUID from attached partitions"));
			break ;
		default:UIMessage(tr("ERROR"),tr( "un unexpected error has occured, key not removed "));
	}
	enableAll();
}

void luksdeletekey::pbOpenVolume()
{
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("volume path"),
						 QDir::homePath(),
						 0);
	m_ui->lineEditVolumePath->setText( Z );
}

void luksdeletekey::deleteKey(QString path)
{
	ShowUI() ;
	m_ui->lineEditVolumePath->setText(path);
}

void luksdeletekey::HideUI()
{
	this->hide();
	emit HideUISignal(this);	
}

luksdeletekey::~luksdeletekey()
{
	delete m_openPartition ;
	delete m_ui ;
}
