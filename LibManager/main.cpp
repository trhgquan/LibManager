#include "include.h"
#include "account.h"


// LUU Y:
// File ACCOUNT_FILE phai co dang nhu sau:

// n - Tong so user
// user1username
// user1password
// user2username
// user2password
// ..
// usernusername
// usernpassword

// Ham nap danh sach nguoi dung & danh sach thong tin nguoi dung
// tu file vao linked list.
void Initialise(accountList*& users, userInfoList*& infos, int& totalAccounts) {
    users = getAccountList(totalAccounts);
    infos = getUserInfoList(totalAccounts);

    if (users == NULL || infos == NULL) {
        cout << "Co van de xay ra voi file du lieu. Vui long tai lai chuong trinh.\n";
        exit(0);
    }
}

// Ham ghi cac danh sach tro lai file sau khi ket thuc.
void Terminate(accountList* users, userInfoList* infos, int totalAccounts) {
    // Ghi danh sach users tro lai file.
    FILE* f = fopen(ACCOUNT_FILE, "wb+");
    fseek(f, 0, SEEK_SET);
    
    // Ghi tong so user.
    fwrite(&totalAccounts, sizeof(int), 1, f);

    // Ghi credentials tung user.
    accountNode* thisNode = users->head;
    while (thisNode != NULL) {
        fwrite(thisNode->credentials, sizeof(account), 1, f);
        thisNode = thisNode->nextAccount;
    }
    fclose(f);

    // Ghi info users tro lai file.
    f = fopen(USER_INFO_FILE, "wb+");

    // Ghi info tung user.
    userInfoNode* thisUserInfo = infos->head;
    while (thisUserInfo != NULL) {
        fwrite(thisUserInfo->info, sizeof(user_info), 1, f);
        thisUserInfo = thisUserInfo->nextUser;
    }
    fclose(f);

    //
    // Xoa user list va user info list.
    //
    freeAccountList(users);
    freeUserInfoList(infos);
}

int main() {
    //
    // Thong tin demo
    //
    cout << "(Demo only): Tai khoan admin: admin/admin@fit.hcmus" << endl;
    cout << "(Demo only): Tai khoan quan ly: lttan/lttan@bqpvietnam" << endl;
    cout << "(Demo only): Tai khoan chuyen vien: thquan/quan@hcmus" << endl;
    system("pause");

    //
    // De bat dau, dau tien load tat ca user va info vao mot danh sach.
    // phuc vu muc dich dang nhap.
    //
    
    int totalAccounts;
    accountList* users = NULL;
    userInfoList* infos = NULL;


    Initialise(users, infos, totalAccounts);

    //
    // Sau do cac bien sau se luu session (phien dang nhap) cua nguoi dung:
    //
    // user_session_account luu account credentials,
    // su dung khi can thay doi password.
    //
    // user_session_info luu info nguoi dung,
    // dung de tra cuu permission, thong tin cung nhu thay doi thong tin.
    //
    // Den day neu can kiem tra dang nhap: truy van xem truong
    // user_session_info = NULL => chua dang nhap, nguoc lai la da dang nhap/
    //

    account* user_session_account = NULL;
    user_info* user_session_info = NULL;
    bool stop_executing = false;
    
    // Bat nguoi dung dang nhap bang moi gia.
    do {
        // Thoat hoan toan chuong trinh.
        if (stop_executing) {
            Terminate(users, infos, totalAccounts);
            break;
        }

        system("cls");
        do {
            user_session_account = logUserIn(users);
            if (user_session_account == NULL) {
                cout << "Sai tai khoan / mat khau. Xin thu lai!" << endl;
            }
            else {
                user_session_info = getUserInfo(infos, user_session_account);

                // Neu nguoi dung khong con active, thong bao va dang xuat.
                if (!isActive(user_session_info)) {
                    cout << "Tai khoan cua ban da bi block. Vui long lien he admin de duoc giai quyet. Xin cam on." << endl;
                    logUserOut(user_session_account, user_session_info);
                    break;
                }

                cout << "Dang nhap thanh cong. Dang chuyen ban den menu nguoi dung phu hop..." << endl;
            }
            system("pause");
            system("cls");
        } while (user_session_account == NULL || user_session_info == NULL);

        // Da dang nhap thanh cong.
        int command_code = -1;
        while (user_session_account != NULL && user_session_info != NULL) {
            // Xoa man hinh.
            system("cls");

            // Load menu co ban.
            generalMenu();
            if (isAdmin(user_session_info)) {
                adminMenu();
            }

            else if (isQuanLy(user_session_info)) {
                quanlyMenu();
                // Bat su kien quan ly menu.
            }

            else {
                chuyenvienMenu();
                // Bat su kien chuyen vien menu.
            }

            cout << "Nhap lenh ban muon thuc hien (so dung truoc moi menu): " << endl;
            if (!(cin >> command_code)) {
                cin.clear(); // xoa loi
                cin.ignore(); // xoa noi dung trong stream
                continue;
            }

            switch (command_code) {
                // Bat su kien doi mat khau.
                case CHANGE_PASSWORD_COMMAND_CODE: {
                    if (changeUserPassword(user_session_account)) {
                        logUserOut(user_session_account, user_session_info);
                        break;
                    }
                    break;
                }

                // Bat su kien doi thong tin.
                case CHANGE_INFO_COMMAND_CODE: {
                    editInfoMenu();
                    catchEditInfoMenu(user_session_account, user_session_info);
                    break;
                }

                // Bat su kien dang xuat.
                case LOGOUT_COMMAND_CODE: {
                    logUserOut(user_session_account, user_session_info);
                    cout << "Dang xuat thanh cong. De thoat hoan toan, hay dang nhap lai va dung lenh " << EXIT_COMMAND_CODE << endl;
                    break;
                }

                // Bat su kien dang xuat va thoat.
                case EXIT_COMMAND_CODE: {
                    if (isLoggedIn(user_session_account, user_session_info))
                        logUserOut(user_session_account, user_session_info);
                    cout << "Da dang xuat, dang thoat chuong trinh.." << endl;
                    stop_executing = true;
                    break;
                }

                case MENU_THEM_USER_COMMAND_CODE: {
                    if (isAdmin(user_session_info)) {
                        if (addUser(totalAccounts, users, infos)) {
                            cout << "Da tao tai khoan moi thanh cong." << endl;
                            cout << "Moi thay doi se duoc luu sau khi dang xuat va thoat khoi chuong trinh bang lenh " << EXIT_COMMAND_CODE << endl;

                            cout << "Thong tin tai khoan moi duoc tao: " << endl;
                            cout << "ID: " << infos->tail->info->ID << endl;
                            cout << "Tai khoan: " << users->tail->credentials->user_name << endl;
                            cout << "Mat khau: " << users->tail->credentials->pass_word << endl;
                        }
                        else cout << "Do dai mot trong so cac thong tin khong hop le." << endl;
                    }
                    else cout << "Ban khong co quyen thuc hien lenh nay." << endl;
                    break;
                }

                case MENU_PHAN_QUYEN_USER_COMMAND_CODE: {
                    if (isAdmin(user_session_info)) {
                        if (permissionUser(infos)) {
                            cout << "Da phan quyen thanh cong." << endl;
                            cout << "Moi thay doi se duoc luu sau khi dang xuat va thoat khoi chuong trinh bang lenh " << EXIT_COMMAND_CODE << endl;
                        }
                        else {
                            cout << "Khong the phan quyen cho user nay vi mot trong so cac ly do sau:" << endl;
                            cout << "- khong ton tai quyen duoc phan cong." << endl;
                            cout << "- nguoi dung duoc phan quyen khong ton tai." << endl;
                            cout << "- phan quyen cho admin." << endl;
                        }
                    }
                    else cout << "Ban khong co quyen thuc hien lenh nay." << endl;
                    break;
                }

                default:
                    cout << "Khong tim thay lenh " << command_code << endl;
                    break;
            }
            system("pause");
        }
    } while (true);
    
    
    return 0;
}