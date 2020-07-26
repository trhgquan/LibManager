#include"include.h"

// Ham tao mot danh sach doc gia (doubly-linked-list).
danhSachDocGia* createDanhSachDocGia() {
    danhSachDocGia* newList = (danhSachDocGia*)malloc(sizeof(danhSachDocGia));
    newList->docGiaDau = NULL;
    newList->docGiaCuoi = NULL;
    return newList;
}

// Ham tao mot node doc gia.
nodeDocGia* createDocGiaNode(theDocGia thongTinDocGia) {
    nodeDocGia* newNode = (nodeDocGia*)malloc(sizeof(nodeDocGia));
    newNode->docGiaPhiaTruoc = NULL;
    newNode->docGiaTiepTheo = NULL;
    newNode->thongTinDocGia = thongTinDocGia;
    return newNode;
}

// Ham doc danh sach doc gia vao mot doubly-linked-list.
danhSachDocGia* getDanhSachDocGiaList(int& totalDocGia) {
    FILE* f = fopen(DOCGIA_FILE, "rb+");

    if (f == NULL) return NULL;

    fseek(f, 0, SEEK_SET);
    fread(&totalDocGia, sizeof(int), 1, f);
    
    danhSachDocGia* newList = createDanhSachDocGia();
    for (int i = 1; i <= totalDocGia; ++i) {
        theDocGia* newTheDocGia = (theDocGia*)malloc(sizeof(theDocGia));
        fread(newTheDocGia, sizeof(theDocGia), 1, f);
        nodeDocGia* newTheDocGiaNode = createDocGiaNode(*newTheDocGia);
        if (i == 1) {
            newList->docGiaDau = newTheDocGiaNode;
            newList->docGiaCuoi = newTheDocGiaNode;
        }
        else {
            newList->docGiaCuoi->docGiaTiepTheo = newTheDocGiaNode;
            newTheDocGiaNode->docGiaPhiaTruoc = newList->docGiaCuoi;
            newList->docGiaCuoi = newTheDocGiaNode;
        }
    }

    return newList;
}

bool isNam(theDocGia docGia) {
    return docGia.gioiTinh == 1;
}

bool isCSV(char const* fileName) {
    return fileName[strlen(fileName) - 1] == 'v' &&
        fileName[strlen(fileName) - 2] == 's' &&
        fileName[strlen(fileName) - 3] == 'c';
}

bool validateDocGiaInfo(theDocGia newDocGia, danhSachDocGia* listDocGia) {
    return strlen(newDocGia.ho_Ten) <= NAME_MAX && strlen(newDocGia.ho_Ten) >= NAME_MIN &&
        strlen(newDocGia.so_CMND) <= CMND_MAX && strlen(newDocGia.so_CMND) >= CMND_MIN &&
        strlen(newDocGia.email) <= EMAIL_MAX && strlen(newDocGia.email) >= EMAIL_MIN &&
        strlen(newDocGia.diaChi) <= ADDRESS_MAX && strlen(newDocGia.diaChi) >= ADDRESS_MIN &&
        searchForDocGiaByCMND(newDocGia.so_CMND, listDocGia) == NULL;
}

void themDocGiaTuCSV(FILE* csvFile, danhSachDocGia*& list, int& totalDocGia) {
    if (csvFile == NULL) return;

    char buffer[1024];
    int index = 0;
    int lineNumber = 1;
    while (fgets(buffer, 1024, csvFile)) {
        theDocGia newDocGia;
        // Doc 1 dong.
        char* cell = strtok(buffer, ",");
        while (cell) {
            if (index == 0) strcpy(newDocGia.ho_Ten, cell);
            if (index == 1) strcpy(newDocGia.so_CMND, cell);
            if (index == 2) strcpy(newDocGia.ngay_Sinh, cell);
            if (index == 3) newDocGia.gioiTinh = (strcmp(cell, "1") == 0) ? 1 : 0;
            if (index == 4) strcpy(newDocGia.email, cell);
            if (index == 5) strcpy(newDocGia.diaChi, cell);
            ++index;

            cell = strtok(NULL, ",");

            if (index == 6) {
                if (validateDocGiaInfo(newDocGia, list)) {
                    time_t now = time(NULL);

                    // Chen ma doc gia va ngay lap the.
                    newDocGia.maDocGia = ++totalDocGia;
                    newDocGia.ngayLapThe = now;

                    // Ngay het han la 2 nam ke tu ngay lap the.
                    // Convert now thanh time struct.
                    tm* timeStruct = gmtime(&now);

                    // Them 2 nam
                    timeStruct->tm_year += 2;

                    // Convert lai ve time_t
                    now = mktime(timeStruct);

                    // Gan cho newDocGia.
                    newDocGia.ngayHetHan = now;

                    // Tao node doc gia moi.
                    nodeDocGia* newNodeDocGia = createDocGiaNode(newDocGia);

                    //
                    // O day ta khong can confirm box,
                    // vi khi user nhap vao file .csv la user da thuc su muon thuc hien.
                    // Ngoai ra, neu co sai sot thi van co the chinh sua tu phia chuyen vien va xoa tu phia quan ly,
                    // neu trung thi da co bao loi.
                    //

                    if (list->docGiaCuoi == NULL) {
                        // List chua co ai.
                        list->docGiaCuoi = newNodeDocGia;
                        list->docGiaDau = newNodeDocGia;
                    }
                    else {
                        // Chen node doc gia vao cuoi list.
                        list->docGiaCuoi->docGiaTiepTheo = newNodeDocGia;
                        newNodeDocGia->docGiaPhiaTruoc = list->docGiaCuoi;
                        list->docGiaCuoi = newNodeDocGia;
                    }       
                }
                else {
                    cout << "Da co loi xay ra o dong so " << lineNumber << endl;
                    cout << "Nguyen nhan co the vao cac loi sau:" << endl;
                    cout << "- CMND da duoc su dung cho mot doc gia khac." << endl;
                    cout << "- Do dai cac thong tin khong phu hop." << endl;
                    cout << "- Loi nhap lieu tu file .csv" << endl;
                }

                index = 0;
                ++lineNumber;
            }
        }
    }
}

nodeDocGia* searchForDocGiaByCMND(const char* CMND, danhSachDocGia* list) {
    nodeDocGia* thisNode = list->docGiaDau;
    while (thisNode != NULL) {
        if (strcmp(thisNode->thongTinDocGia.so_CMND, CMND) == 0)
            return thisNode;
        thisNode = thisNode->docGiaTiepTheo;
    }

    return NULL;
}

void searchForDocGiaByHoTen(const char* HoTen, danhSachDocGia* list) {
    nodeDocGia* thisNode = list->docGiaDau;
    bool foundDocGia = false;
    while (thisNode != NULL) {
        char* tmp_name = _strdup(thisNode->thongTinDocGia.ho_Ten);
        if (strcmp(_strlwr(tmp_name), HoTen) == 0) {
            inDocGia(thisNode->thongTinDocGia);
            foundDocGia = true;

            // Khong break vi co the co nhieu doc gia trung ten.
            // chi co flag danh dau xem co tim thay hay khong.
        }
        thisNode = thisNode->docGiaTiepTheo;
    }

    if (!foundDocGia) cout << "Khong tim thay doc gia co ten " << HoTen << endl;
}

void xemDanhSachDocGia(nodeDocGia* docGia) {
    if (docGia != NULL) {
        theDocGia thongTin = docGia->thongTinDocGia;
        inDocGia(thongTin);

        if (docGia->docGiaTiepTheo != NULL)
            xemDanhSachDocGia(docGia->docGiaTiepTheo);
    }
    else cout << "Hien tai khong co doc gia nao trong danh sach." << endl;
}

void thongKeTheoGioiTinh(danhSachDocGia* dsDocGia, int tongDocGia) {
    nodeDocGia* thisNode = dsDocGia->docGiaDau;
    int docGiaNam = 0;

    while (thisNode != NULL) {
        if (isNam(thisNode->thongTinDocGia)) ++docGiaNam;
        thisNode = thisNode->docGiaTiepTheo;
    }

    cout << "Thong ke theo gioi tinh:" << endl;
    cout << "Co tong cong " << tongDocGia << " doc gia, trong do: " << endl;
    cout << docGiaNam << " nguoi la NAM." << endl;
    cout << tongDocGia - docGiaNam << " nguoi la NU." << endl;
}

void inDocGia(theDocGia thongTin) {
    printf("ID: %d\n", thongTin.maDocGia);
    printf("Ho ten: %s\n", thongTin.ho_Ten);
    printf("Email: %s\n", thongTin.email);
    printf("Gioi tinh: %s\n", (thongTin.gioiTinh == NAM) ? "NAM" : "NU");
    printf("CMND: %s\n", thongTin.so_CMND);
    printf("Ngay sinh: %s\n", thongTin.ngay_Sinh);
    printf("Dia chi: %s\n", thongTin.diaChi);
    printf("Ngay lap the: %s\n", ctime(&thongTin.ngayLapThe));
    printf("Ngay het han: %s\n", ctime(&thongTin.ngayHetHan));
}

void deleteDocGiaDau(danhSachDocGia*& list) {
    nodeDocGia* headNode = list->docGiaDau;

    if (headNode->docGiaTiepTheo == NULL)
        list->docGiaDau = NULL;
    else list->docGiaDau = headNode->docGiaTiepTheo;

    free(headNode);
}

void freeDanhSachDocGia(danhSachDocGia*& list) {
    while (list->docGiaDau != NULL)
        deleteDocGiaDau(list);
    free(list);
}