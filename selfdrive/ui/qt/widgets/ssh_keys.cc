#include "selfdrive/ui/qt/widgets/ssh_keys.h"

#include "common/params.h"
#include "selfdrive/ui/qt/api.h"
#include "selfdrive/ui/qt/widgets/input.h"

SshControl::SshControl() : ButtonControl(SshControl::tr("SSH Keys"), "", SshControl::tr("Warning: This grants SSH access to all public keys in your GitHub settings.")) {
  username_label.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  username_label.setStyleSheet("color: #aaaaaa");
  hlayout->insertWidget(1, &username_label);

  QObject::connect(this, &ButtonControl::clicked, [=]() {
    if (text() == SshControl::tr("ADD")) {
      QString username = InputDialog::getText(SshControl::tr("Enter your GitHub username"), this);
      if (username.length() > 0) {
        setText(SshControl::tr("LOADING"));
        setEnabled(false);
        getUserKeys(username);
      }
    } else {
      params.remove("GithubUsername");
      params.remove("GithubSshKeys");
      refresh();
    }
  });

  refresh();
}

void SshControl::refresh() {
  QString param = QString::fromStdString(params.get("GithubSshKeys"));
  if (param.length()) {
    username_label.setText(QString::fromStdString(params.get("GithubUsername")));
    setText(SshControl::tr("REMOVE"));
  } else {
    username_label.setText("");
    setText(SshControl::tr("ADD"));
  }
  setEnabled(true);
}

void SshControl::getUserKeys(const QString &username) {
  HttpRequest *request = new HttpRequest(this, false);
  QObject::connect(request, &HttpRequest::requestDone, [=](const QString &resp, bool success) {
    if (success) {
      if (!resp.isEmpty()) {
        params.put("GithubUsername", username.toStdString());
        params.put("GithubSshKeys", resp.toStdString());
      } else {
        ConfirmationDialog::alert(QString(SshControl::tr("Username '%1' has no keys on GitHub")).arg(username), this);
      }
    } else {
      if (request->timeout()) {
        ConfirmationDialog::alert("Request timed out", this);
      } else {
        ConfirmationDialog::alert(QString(SshControl::tr("Username '%1' doesn't exist on GitHub")).arg(username), this);
      }
    }

    refresh();
    request->deleteLater();
  });

  request->sendRequest("https://github.com/" + username + ".keys");
}
