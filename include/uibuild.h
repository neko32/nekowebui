#pragma once
#ifndef __UI_BUILD_H__
#define __UI_BUILD_H__

#include <string>
#include <vector>
#include <sstream>
#include <optional>
#include <format>
using namespace std;

namespace tanu::webui::builder {

    static const string BOOTSTRAP_VERSION = "5.3.3";
    static const string BOOTSTRAP_TABLE_VERSION = "1.23.5";
    static const string JQUERY_VERSION = "3.7.1";

    static const string BOOTSTRAP_CSS_HREF = format("https://cdn.jsdelivr.net/npm/bootstrap@{}/dist/css/bootstrap.min.css", BOOTSTRAP_VERSION);
    static const string BOOTSTRAP_CSS_HASH = "sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH";
    static const string BOOTSTRAP_SCRIPT_HREF = format("https://cdn.jsdelivr.net/npm/bootstrap@{}/dist/js/bootstrap.bundle.min.js", BOOTSTRAP_VERSION);
    static const string BOOTSTRAP_SCRIPT_SHA_HASH = "sha384-YvpcrYf0tY3lHB60NNkmXc5s9fDVZLESaAA55NDzOxhy9GkcIdslK1eN7N6jIeHz";

    static const string BOOTSTRAP_TABLE_CSS_HREF = format("https://cdn.jsdelivr.net/npm/bootstrap-table@{}/dist/bootstrap-table.min.css", BOOTSTRAP_TABLE_VERSION);
    static const string BOOTSTRAP_TABLE_SCRIPT_HREF = format("https://cdn.jsdelivr.net/npm/bootstrap-table@{}/dist/bootstrap-table.min.js", BOOTSTRAP_TABLE_VERSION);
    static const string BOOTSTRAP_TABLE_LOCALE_HREF = format("https://cdn.jsdelivr.net/npm/bootstrap-table@{}/dist/locale/bootstrap-table-zh-CN.min.js", BOOTSTRAP_TABLE_LOCALE_HREF);

    static const string JQUERY_HREF = format("https://code.jquery.com/jquery-{}.min.js", JQUERY_VERSION);

    class BuildUtil {
    public:
        static string gentabs(int n) {
            stringstream ss;
            for(int i = 0;i < n;++i) ss << "\t";
            return ss.str();
        }

        static void install_favicon(const string& dest_path, const optional<string> src_path = nullopt);
        
    };

    enum class Flavor {
        Bootstrap,
    };

    class Page {
    protected:
        int m_main_width = 640;
        optional<string> m_main_logo_path = nullopt;
        string m_base_color_1 = "#5e9ac9";
        string m_base_color_2 = "#0087c4";
        string m_accent_color = "#f6be09";
        string m_title = "Tanu Web UI";
        string m_meta_charset = "UTF-8";
        string m_css_file_path = "css/base.css";
        vector<string> m_font_families {
            "Arial", "Noto Sans JP", "ヒラギノ角ゴ ProN W3", "Hiragino Kaku Gothic ProN",
            "Yu Gothic", "#YuGothic",
            "メイリオ", "#Meiryo",
            "ＭＳ ゴシック", "#sans-serif"
        };
        Flavor m_flavor = Flavor::Bootstrap;
        bool m_skip_header = false;
        bool m_use_css = true;
        bool m_enable_favicon = true;

        string font_families_as_str();
        
    public:
        Page() {}
        void skip_header(bool skip_header) {
            this->m_skip_header = skip_header;
        }
        void use_css(bool use_css) {
            this->m_use_css = use_css;
        }
        void enable_favicon(bool enable_favicon) {
            this->m_enable_favicon = enable_favicon;
        }
        void main_log_path(const string& main_log_path) {
            this->m_main_logo_path = main_log_path;
        }
        void base_color_1(const string& base_color) {
            this->m_base_color_1 = base_color;
        }
        void base_color_2(const string& base_color) {
            this->m_base_color_2 = base_color;
        }
        void accent_color(const string& accent_color) {
            this->m_accent_color = accent_color;
        }
        void title(const string& title) {
            this->m_title = title;
        }
        void flavor(Flavor flavor) {
            this->m_flavor = flavor;
        }
        void css_file_path(const string css_file_path) {
            this->m_css_file_path = css_file_path;
        }
        void font_families(const vector<string>& font_families) {
            this->m_font_families = font_families;
        }

        string start();
        string end();
        void generate_css(const string& install_path);

        friend class Element;
        friend class UIBuilder;
    };

    class Body {
    private:
        stringstream m_ss;
        int m_num_tabs = 1;
    public:
        Body(){}
        void start(optional<string> cls = nullopt);
        void end();
        void h1(const string& h1_str);
        void paragraph(const string& p_str);
        void num_tabs(int num_tabs) {this->m_num_tabs = num_tabs;}
        int num_tabs() {return this->m_num_tabs;}
        string gentabs(){ return BuildUtil::gentabs(this->m_num_tabs);}
        void incr_tab() {this->m_num_tabs++;}
        void decr_tab() {this->m_num_tabs--;}
        void start_div();
        void end_div();
        void text_search_input(const string& id, const string& placeholder);
        void inject(string s, bool with_line_break = true) { 
            this->m_ss << s;
            if(with_line_break) this->m_ss << endl;
        }
        string str() { return this->m_ss.str(); }
    };

    class Table {
    protected:
        int m_num_tabs = 1;
        string m_table_body_id = "table_body";
        vector<string> m_cols;
        vector<vector<string>> m_rows;
        string gentabs(){ return BuildUtil::gentabs(this->m_num_tabs);}
        void incr_tab() {this->m_num_tabs++;}
        void decr_tab() {this->m_num_tabs--;}
    public:
        Table() {}
        Table(int num_tabs):m_num_tabs(num_tabs){}
        Table(int num_tabs, const string& table_body_id):m_num_tabs(num_tabs), m_table_body_id(table_body_id){}
        void columns(vector<string> cols){ this->m_cols = cols;}
        void add_row(vector<string> row) {this->m_rows.push_back(row);};
        virtual string str();
    };
    
    class SearchableTable: public Table {
    protected:
        string m_search_input_id;
        string m_table_body_id;
        const string IPT_KEYWORD = "__IPT__";
        const string TBL_KEYWORD = "__TBL__";
    public:
        SearchableTable(const string& search_input_id, const string& table_body_id):Table(), m_search_input_id(search_input_id), m_table_body_id(table_body_id) {}
        string str() override;
    };

    class Nav {
    private:
        int m_num_tabs = 3;
        optional<string> m_brand_text = "Tanu Web UI";
        optional<string> m_brand_img = "img/icon-32x32.png";
        string gentabs(){ return BuildUtil::gentabs(this->m_num_tabs);}
        void incr_tab() {this->m_num_tabs++;}
        void decr_tab() {this->m_num_tabs--;}
    public:
        Nav() {}
        Nav(int num_tabs):m_num_tabs(num_tabs){}
        void brand_text(optional<string> brand_text) { this->m_brand_text = brand_text;}
        void brand_img(optional<string> brand_img) { this->m_brand_img = brand_img;}
        string str();
    };

}

#endif