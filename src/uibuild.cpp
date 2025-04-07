#include "uibuild.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <format>
#include <stdexcept>
#include <filesystem>
using namespace std;

namespace tanu::webui::builder {

    void BuildUtil::install_favicon(const string& dest_path, const optional<string> src_path) {
        string src_real_path {filesystem::path(getenv("NEKORC_PATH")) / "nekowebui" / "img" };
        if(src_path != nullopt) {
            src_real_path = src_path.value();
        }
        if(!filesystem::exists(dest_path)) {
            if(!filesystem::create_directories(dest_path)) {
                throw runtime_error(format("creating {} failed", dest_path));
            }
        }

        if(!filesystem::exists(filesystem::path(dest_path) / "favicon.ico")) {
            filesystem::copy_file(filesystem::path(src_real_path) / "favicon.ico", filesystem::path(dest_path) / "favicon.ico");
        }
    }

    string Page::font_families_as_str() {
        stringstream ss;
        if(this->m_font_families.size() == 0) {
            throw runtime_error("font family must have one or more fonts");
        }
        ss << "font-family: ";
        for(const string& font_family:this->m_font_families) {
            if(font_family[0] == '#') {
                ss << font_family.substr(1) << ",";
            } else {
                ss << "'" << font_family << "',";
            }
        }
        string t {ss.str()};
        t.pop_back();
        t.push_back(';');
        return t;
    }

    void Page::generate_css(const string& install_path) {
        stringstream ss;

        ss << "BODY {" << endl;
        ss << "\t" << this->font_families_as_str() << endl;
        ss << "}" << endl;

        filesystem::path install_path_obj {install_path};
        filesystem::path parent_dir = install_path_obj.parent_path();
        if(!filesystem::exists(parent_dir)) {
            if(!filesystem::create_directories(parent_dir)) {
                throw runtime_error(format("failed to create {}", parent_dir.string()));
            }
        }
        
        ofstream ofs;
        ofs.open(install_path);
        if(!ofs.is_open()) {
            throw runtime_error(format("{} failed to open", install_path));
        }
        ofs << ss.str() << endl;
        ofs.close();

    }

    string Page::start() {
        stringstream ss;
        ss << "<HTML>" << endl;
        ss << "\t<HEAD>" << endl;

        // TITLE
        ss << format("\t\t<TITLE>{}</TITLE>", this->m_title) << endl;

        // FAVICON
        if(this->m_enable_favicon) {
            ss << "\t\t<LINK rel=\"icon\" href=\"img/favicon.ico\"/>" << endl;
        }

        // META
        ss << format("\t\t<META charset=\"{}\"/>", this->m_meta_charset) << endl;

        // CSS & JS
        if(this->m_use_css) {
            ss << format("\t\t<LINK rel=\"stylesheet\" href=\"{}\"/>", this->m_css_file_path) << endl;
            if(this->m_flavor == Flavor::Bootstrap) {
                ss << format("\t\t<LINK href=\"{}\" rel=\"stylesheet\" integrity=\"{}\" crossorigin=\"anonymous\"/>", BOOTSTRAP_CSS_HREF, BOOTSTRAP_CSS_HASH) << endl;
            }
        }
        if(this->m_flavor == Flavor::Bootstrap) {
            ss << format("\t\t<SCRIPT src=\"{}\" integrity=\"{}\" crossorigin=\"anonymous\"></SCRIPT>", BOOTSTRAP_SCRIPT_HREF, BOOTSTRAP_SCRIPT_SHA_HASH) << endl;
        }

        ss << format("\t\t<SCRIPT src=\"{}\"></SCRIPT>", JQUERY_HREF) << endl;
        
        ss << "\t</HEAD>" << endl;

        return ss.str();
    }

    string Page::end() {
        stringstream ss;
        ss << "</HTML>" << endl;
        return ss.str();
    }

    void Body::start(optional<string> cls) {
        if(cls == nullopt) {
            this->m_ss << format("{}<BODY>", this->gentabs()) << endl;
        } else {
            this->m_ss << format("{}<BODY class=\"{}\">", this->gentabs(), cls.value()) << endl;
        }
        this->incr_tab();
    }

    void Body::end() {
        this->decr_tab();
        this->m_ss << format("{}</BODY>", this->gentabs()) << endl;
    }

    void Body::h1(const string& h1_str) {
        this->m_ss << format("{}<H1>{}</H1>", this->gentabs(), h1_str) << endl;
    }

    void Body::paragraph(const string& p_str) {
        this->m_ss << format("{}<P>{}</P>", this->gentabs(), p_str) << endl;
    }

    void Body::start_div() {
        this->m_ss << format("{}<DIV>", this->gentabs()) << endl;
        this->incr_tab();
    }

    void Body::end_div() {
        this->decr_tab();
        this->m_ss << format("{}</DIV>", this->gentabs()) << endl;
    }

    void Body::text_search_input(const string& id, const string& placeholder) {
        this->m_ss << format("{}<INPUT id=\"{}\" type=\"text\" placeholder=\"{}\"/>", this->gentabs(), id, placeholder) << endl;
    }

    string Table::str() {
        stringstream ss;    
        if(this->m_cols.size() == 0 || this->m_rows.size() == 0) {
            throw runtime_error("Col and/or Row empty");
        }
        // TABLE
        ss << format("{}<TABLE class=\"table table-hover\">", this->gentabs()) << endl;
        this->incr_tab();

        // TABLE HEADER
        ss << this->gentabs() << "<THEAD>" << endl;
        this->incr_tab();
        ss << this->gentabs() << "<TR>" << endl;
        this->incr_tab();
        for(const string& col:this->m_cols) {
            ss << format("{}<TH scope=\"col\">{}</TH>", this->gentabs(), col) << endl;
        }
        this->decr_tab();
        ss << this->gentabs() << "</TR>" << endl;
        this->decr_tab();
        ss << this->gentabs() << "</THEAD>" << endl;
        ss << format("{}<TBODY id=\"{}\">", this->gentabs(), this->m_table_body_id) << endl;
        this->incr_tab();

        int row_n = 1;
        
        for(const vector<string>& row:this->m_rows) {
            ss << format("{}<TR>", this->gentabs()) << endl;    
            this->incr_tab();

            ss << format("{}<TH scope=\"row\">{}</TH>", this->gentabs(), row_n++) << endl;
            
            for(const string& row_elem:row) {
                ss << format("{}<TD>{}</TD>", this->gentabs(), row_elem) << endl;
            }
            this->decr_tab();
            ss << format("{}</TR>", this->gentabs()) << endl;
        }

        this->decr_tab();
        ss << format("{}</TBODY>", this->gentabs()) << endl;
        this->decr_tab();
        ss << format("{}</TABLE>", this->gentabs()) << endl;

        return ss.str();

    }

    string SearchableTable::str() {
        stringstream ss;
        string nekorc_path = getenv("NEKORC_PATH");
        filesystem::path js_file_path {nekorc_path};
        js_file_path = js_file_path / "nekowebui" / "js" / "search_table.js";
        ifstream ifs;
        ifs.open(js_file_path.string());
        if(!ifs.is_open()) {
            throw runtime_error(format("failed to open {}", js_file_path.string()));
        }
        string table_js{};
        string t{};
        stringstream iss;
        while(getline(ifs, t)) {
            iss << t << endl;
        }
        table_js = iss.str();
        ifs.close();
        
        auto ipt_key_loc = table_js.find(this->IPT_KEYWORD);
        table_js.replace(ipt_key_loc, this->IPT_KEYWORD.length(), this->m_search_input_id);
        auto tbl_key_loc = table_js.find(this->TBL_KEYWORD);
        table_js.replace(tbl_key_loc, this->TBL_KEYWORD.length(), this->m_table_body_id);

        ss << format("{}<SCRIPT>", this->gentabs()) << endl;
        ss << endl << table_js << endl; 
        ss << format("{}</SCRIPT>", this->gentabs()) << endl; 
        return ss.str();
    }

    string Nav::str() {
        stringstream ss;
        ss << format("{}<NAV class=\"navbar bg-body-tertiary\">", this->gentabs()) << endl;
        this->incr_tab();
        ss << format("{}<DIV class=\"container-fluid\">", this->gentabs()) << endl;
        this->incr_tab();
        ss << format("{}<A class=\"navbar-brand\" href=\"#\">", this->gentabs()) << endl;
        this->incr_tab();
        if(this->m_brand_img != nullopt) {
            ss << format("{}<IMG src=\"{}\" alt=\"Logo\" width=\"32\" height=\"32\" class=\"d-inline-block align-text-top\"/>", this->gentabs(), this->m_brand_img.value()) << endl;
        }
        if(this->m_brand_text != nullopt) {
            ss << format("{}{}", this->gentabs(), this->m_brand_text.value()) << endl;
        }
        this->decr_tab();
        ss << format("{}</A>", this->gentabs()) << endl;
        this->decr_tab();
        ss << format("{}</DIV>", this->gentabs()) << endl;
        this->decr_tab();
        ss << format("{}</NAV>", this->gentabs()) << endl;
        return ss.str();
    }

}