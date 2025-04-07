#include "uibuild.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;
using namespace tanu::webui::builder;

int main() {
    stringstream ss;
    Page p;
    p.use_css(true);
    p.title("ねこたこの世界");

    p.generate_css("/mnt/c/tmp/takona/css/base.css");
    BuildUtil::install_favicon("/mnt/c/tmp/takona/img");

    ss << p.start();

    SearchableTable stbl {"word_dict_search_ipt", "word_dict_items"};
    ss << stbl.str() << endl;

    Body b;
    b.start();
    
    Nav nav(b.num_tabs());
    b.inject(nav.str(), false);

    b.h1("TAKO");
    b.paragraph("これはnekoですか？");
    b.paragraph("&nbsp;ねこですね。");

    b.start_div();
    b.paragraph("中の章１");
    b.end_div();

    b.text_search_input("word_dict_search_ipt", "Search...");

    Table tbl(b.num_tabs(), "word_dict_items");
    tbl.columns({"項目", "説明"});
    tbl.add_row({"アダマール積", "２つ集合のそれぞれ対応する(同じ行・列の位置)の要素の積で構成される新たな行列を得る。結合的、可換的、分配的。"});
    tbl.add_row({"1日の秒", "86,000秒 / 日"});
    b.inject(tbl.str(), false);

    b.end();
    ss << b.str();

    ss << p.end();

    ofstream ofs;
    ofs.open("/mnt/c/tmp/takona/takona.html");
    if(!ofs.is_open()) {
        cerr << "failed to open file" << endl;
        exit(1);
    }
    ofs << ss.str() << endl;
    ofs.close();

}