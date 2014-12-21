/**
* Created:  2014-08-19
* Author:   Oscar Campbell
* Licence:  MIT (Expat) - http://opensource.org/licenses/mit-license.html
**/



    namespace Bettie {


    class Component {


        vector<Component*>      children;


    };

    class Doc : public Bettie::Component {

    }


    namespace Web {

    class LoginForm : public Bettie::Component {

        void init() {
            //asquery(&records, "SELECT * FROM news_roster ORDER BY time DESC LIMIT 10");
            asquery(&records, "GET TOP 10 news_roster BY time");

            parent::init()

        }

        void render() {
            BettieBuf out;

            for( auto rs : records ) {
                out << form_tpl.render(rs);

            }

            up('content')(out);

            parent::render();

        }

        Tpl form_tpl = """
    <div id="{{the_id | "undefined"}}>
        {{content | "No news" }}
    </div>
    """

    }





    layout()

        doc(
            head(
                scripts,
                css,
                metas
            ),
            body(
                main_content
            )
        );
