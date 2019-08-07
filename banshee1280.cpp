#include "banshee1280.h"
#include "ui_banshee1280.h"
#include <QFileDialog>
#include <QTreeView>
#include <QDebug>
#include <QDir>
#include <QStatusBar>
#include <QMessageBox>
#include <QPixmap>
#include <QSettings>
#include <QUrl>
#include <QtCore>
#include <QDesktopServices>
#include <QDirIterator>
#include <QMovie>
#include <iostream>


QString Type,Logo;

Banshee1280::Banshee1280(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Banshee1280)
{
    ui->setupUi(this);
    ui->Icon_label->setVisible(false);
    system ("mount /dev/mmcblk0p5 /mnt");
    QString strKeyLocalVersion("File System Configuration/");
    QSettings * config = 0;
    config = new QSettings("/mnt/fsdefs.ini", QSettings::IniFormat );
    ui->FileSystemNameLabel->setText(config->value( strKeyLocalVersion + "Name", "r").toString());
    ui->FileSystemVersionLabel->setText(config->value( strKeyLocalVersion + "Version", "r").toString());
    ui->FileSystemDescriptionLabel->setText(config->value( strKeyLocalVersion + "Description", "r").toString());
    Type = config->value( strKeyLocalVersion + "Type", "r").toString();
    Logo = config->value( strKeyLocalVersion + "Logo", "r").toString();
    if ( QFile("/mnt/"+Logo).exists())
    {
        QPixmap pixmap("/mnt/"+Logo);
        ui->Icon_label->setPixmap(pixmap);
        //ui->Icon_label->setMask(pixmap.mask());
        ui->Icon_label->setVisible(true);
        std::cout << "Pixmap Found : "<< Logo.toLatin1().constData() <<"\n" << std::flush;
    }
    else
        std::cout << "Pixmap not Found\n" << std::flush;
    std::cout << "Found " << Type.toLatin1().constData() << " image\n" << std::flush;

    system ("umount /mnt");
    this->setCursor(Qt::ArrowCursor);
}


Banshee1280::~Banshee1280()
{
    delete ui;
}


#define NUMOPS_LINUX 29
#define NUMOPS_ANDROID7 10

char linux_ops[NUMOPS_LINUX][512] = {
    "umount /src /dest_rootfs ; mkdir /src /dest_rootfs; mount /dev/mmcblk0p5 /src ;\
    sgdisk -Z /dev/mmcblk2; \
    partprobe /dev/mmcblk2 ; \
    dd if=/dev/zero of=/dev/mmcblk2 bs=16M count=1 ; \
    dd if=/src/eMMC_Boot/idbloader.img of=/dev/mmcblk2 seek=64 conv=notrunc,sync,fsync; \
    dd if=/src/eMMC_Boot/uboot.img     of=/dev/mmcblk2 seek=$(((0x002000 + 0x2000))) ibs=1M conv=sync,fsync; \
    dd if=/src/eMMC_Boot/trust.img     of=/dev/mmcblk2 seek=$(((0x004000 + 0x2000))) ibs=1M conv=sync,fsync ; sync",
    "sgdisk -n 1:16M: -c 1:rootfs /dev/mmcblk2 ;  yes | mkfs.ext4 /dev/mmcblk2p1 ; mount /dev/mmcblk2p1 /dest_rootfs ; sleep 1 ; sync",
    "cp -a /src/bin /dest_rootfs/.",
    "cp -a /src/boot /dest_rootfs/.",
    "cp -a /src/dev /dest_rootfs/.",
    "cp -a /src/etc /dest_rootfs/.",
    "cp -a /src/home /dest_rootfs/.",
    "cp -a /src/lib /dest_rootfs/.",
    "cp -a /src/media /dest_rootfs/.",
    "cp -a /src/mnt /dest_rootfs/.",
    "cp -a /src/opt /dest_rootfs/.",
    "cp -a /src/proc /dest_rootfs/.",
    "cp -a /src/root /dest_rootfs/.",
    "cp -a /src/run /dest_rootfs/.",
    "cp -a /src/sbin /dest_rootfs/.",
    "cp -a /src/selinux /dest_rootfs/.",
    "cp -a /src/srv /dest_rootfs/.",
    "cp -a /src/sys /dest_rootfs/.",
    "cp -a /src/tmp /dest_rootfs/.",
    "mkdir /dest_rootfs/usr ; cp -a /src/usr/bin /dest_rootfs/usr/.",
    "cp -a /src/usr/games /dest_rootfs/usr/.",
    "cp -a /src/usr/include /dest_rootfs/usr/.",
    "cp -a /src/usr/lib /dest_rootfs/usr/.",
    "cp -a /src/usr/local /dest_rootfs/usr/.",
    "cp -a /src/usr/sbin /dest_rootfs/usr/.",
    "cp -a /src/usr/share /dest_rootfs/usr/.",
    "cp -a /src/usr/src /dest_rootfs/usr/.",
    "cp -a /src/var /dest_rootfs/.",
    "cp /src/eMMC_Boot/NOVAsomParams /dest_rootfs/boot/. ; cp /src/eMMC_Boot/dtb.dtb /dest_rootfs/boot/. ; \
    cd /dest_rootfs/home/novasomindustries ; \
    chown -R 1000 * ; \
    chgrp -R 1000 * ; \
    chown -R 1000 .* ; \
    chgrp -R 1000 .* ; cd ; \
    umount /src /dest_rootfs ; \
    sync"
};
char linux_visible_text[NUMOPS_LINUX][512] = {
    "Partitioning eMMC",
    "Formatting eMMC and writing boot files",
    "Copying /bin",
    "Copying /boot",
    "Copying /dev",
    "Copying /etc",
    "Copying /home",
    "Copying /lib",
    "Copying /media",
    "Copying /mnt",
    "Copying /opt",
    "Copying /proc",
    "Copying /root",
    "Copying /run",
    "Copying /sbin",
    "Copying /selinux",
    "Copying /srv",
    "Copying /sys",
    "Copying /tmp",
    "Copying /usr/bin",
    "Copying /usr/games",
    "Copying /usr/include",
    "Copying /usr/lib",
    "Copying /usr/local",
    "Copying /usr/sbin",
    "Copying /usr/share ( this is a long process, and depends on uSD speed and eMMC size. Please wait )",
    "Copying /usr/src",
    "Copying /var",
    "Customizing image",
};

/*
    "mount /dev/mmcblk0p5 /mnt ;\
    dd if=/mnt/SD.bin of=/dev/mmcblk2 seek=$(((0x000040 + 0x0000))) conv=sync,fsync; \
    dd if=/mnt/SD.bin            of=/dev/mmcblk2 seek=$(((0x000040 + 0x0400))) conv=sync,fsync; \
    dd if=/mnt/SD.bin            of=/dev/mmcblk2 seek=$(((0x000040 + 0x0800))) conv=sync,fsync; \
    dd if=/mnt/SD.bin            of=/dev/mmcblk2 seek=$(((0x000040 + 0x0C00))) conv=sync,fsync; \
    dd if=/mnt/SD.bin            of=/dev/mmcblk2 seek=$(((0x000040 + 0x1000))) conv=sync,fsync" ,
*/

char android7_ops[NUMOPS_ANDROID7][512] = {
    "mount /dev/mmcblk0p5 /mnt ;  dd if=/mnt/idbloader.img of=/dev/mmcblk2  seek=64 conv=sync,fsync,notrunc",
    "dd if=/mnt/parameter.img     of=/dev/mmcblk2 seek=$(((0x000000 + 0x2000))) ibs=1M conv=sync,fsync" ,
    "dd if=/mnt/uboot.img         of=/dev/mmcblk2 seek=$(((0x002000 + 0x2000))) ibs=1M conv=sync,fsync" ,
    "dd if=/mnt/trust.img         of=/dev/mmcblk2 seek=$(((0x004000 + 0x2000))) ibs=1M conv=sync,fsync" ,
    "dd if=/mnt/misc.img          of=/dev/mmcblk2 seek=$(((0x008000 + 0x2000))) ibs=1M conv=sync,fsync" ,
    "dd if=/mnt/resource.img      of=/dev/mmcblk2 seek=$(((0x00A800 + 0x2000))) ibs=1M conv=sync,fsync" ,
    "dd if=/mnt/kernel.img        of=/dev/mmcblk2 seek=$(((0x012000 + 0x2000))) ibs=1M conv=sync,fsync" ,
    "dd if=/mnt/boot.img          of=/dev/mmcblk2 seek=$(((0x022000 + 0x2000))) ibs=1M conv=sync,fsync" ,
    "dd if=/mnt/recovery.img      of=/dev/mmcblk2 seek=$(((0x032000 + 0x2000))) ibs=1M conv=sync,fsync" ,
    "dd if=/mnt/system.img        of=/dev/mmcblk2 seek=$(((0x0AC000 + 0x2000))) ibs=1M conv=sync,fsync ; sync ; umount /mnt ; sync"
};
char android7_visible_text[NUMOPS_ANDROID7][512] = {
    "Partitioning eMMC and storing boot files",
    "Storing parameter.img",
    "Storing uboot.img",
    "Storing trust.img",
    "Storing misc.img",
    "Storing resource.img",
    "Storing kernel.img",
    "Storing boot.img",
    "Storing recovery.img",
    "Storing system.img ( this is a long process, and depends on uSD speed and eMMC size. Please wait )",
};


void Banshee1280::on_write_pushButton_clicked()
{
    int i=0 , interval , statbarval , found_image=0;
    statbarval = 0;
    this->setCursor(Qt::WaitCursor);

    if ( Type == "Linux")
    {
        interval = 100 / NUMOPS_LINUX;
        found_image = 1;
        for(i=0;i<NUMOPS_LINUX;i++)
        {
            ui->writeop_label->setText(linux_visible_text[i]);
            ui->writeop_label->repaint();
            qApp->processEvents();
            std::cout << "ops : " << linux_ops[i] << "\n" << std::flush;
            system(linux_ops[i]);
            statbarval += interval;
            ui->progressBar->setValue(statbarval);
            ui->progressBar->repaint();
            qApp->processEvents();

        }
    }
    if ( Type == "Android7")
    {
        interval = 100 / NUMOPS_ANDROID7;
        found_image = 1;
        for(i=0;i<NUMOPS_ANDROID7;i++)
        {
            ui->writeop_label->setText(android7_visible_text[i]);
            ui->writeop_label->repaint();
            qApp->processEvents();
            std::cout << "android_ops : " << android7_ops[i] << "\n" << std::flush;
            system(android7_ops[i]);
            statbarval += interval;
            ui->progressBar->setValue(statbarval);
            ui->progressBar->repaint();
            qApp->processEvents();

        }
    }
    if( found_image == 1)
    {
        ui->writeop_label->setText("Finished! Power off your target and remove uSD");
        ui->progressBar->setValue(100);
        this->setCursor(Qt::ArrowCursor);
    }
    else
    {
        ui->writeop_label->setText("No valid image found! No write actions has been performed on eMMC");
        ui->progressBar->setValue(100);
        this->setCursor(Qt::ArrowCursor);
    }
}

