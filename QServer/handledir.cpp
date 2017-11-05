#include <QDir>

//QDir directoryOf(const QString &subdir)
//{
//    QDir dir(QApplication::applicationDirPath());

//#if defined(Q_OS_WIN)
//    if (dir.dirName().toLower() == "debug"
//            || dir.dirName().toLower() == "release"
//            || dir.dirName().toLower() == "bin")
//        dir.cdUp();
//#elif defined(Q_OS_MAC)
//    if (dir.dirName() == "MacOS") {
//        dir.cdUp();
//        dir.cdUp();
//        dir.cdUp();
//    }
//#endif
//    dir.cd(subdir);
//    return dir;
//}
QString getPath()
{
    QDir dir(QCoreApplication::applicationDirPath());

#if defined(Q_OS_WIN)
    if (dir.dirName().toLower() == "debug"
            || dir.dirName().toLower() == "release"
            || dir.dirName().toLower() == "bin") {
        dir.cdUp();
        dir.cdUp();
    }
#elif defined(Q_OS_MAC)
    if (dir.dirName() == "MacOS") {
        dir.cdUp();
        dir.cdUp();
        dir.cdUp();
        dir.cdUp();
        dir.cdUp();
//        dir.cd("Tmpfile");
    }
#endif
    return dir.absolutePath ();
}
