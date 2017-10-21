#include "qclient.h"
#include <QApplication>

#include "signin.h"
#include "signup.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QClient w;
    SignIn signinWindow;
    SignUp *signupWindow;
    signinWindow.id = &w.userID;
    SignIn::DoneType returnType = static_cast<SignIn::DoneType>(signinWindow.exec());
    switch (returnType) {
    case SignIn::SignInSuccess:
        w.show ();
        w.start ();
        return a.exec ();
        break;
    case SignIn::SignUp:
        signupWindow = new SignUp;
        signupWindow->id = &w.userID;
        if (signupWindow->exec () == QDialog::Accepted) {
            signupWindow->deleteLater ();
            w.show ();
            w.start ();
            return a.exec ();
        }
        else {
            signupWindow->deleteLater ();
        }
        break;
    default:
        break;
    }
//    if (signinWindow.exec() == QDialog::Accepted) {
//        w.show ();
//        return a.exec ();
//    }
//    w.show();
//    return a.exec();
    return 0;
}
