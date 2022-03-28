(function ($, window, document) {
    'use strict';

    var $body = $(document.body);

    function detectAddAllMaterial() {
        var $wrapper = $('.electrochild-material'),
            $btn = $wrapper.find('.add_all_to_cart_button');

        function onAddAllClicked(e) {
            e.preventDefault();
            if ($btn.hasClass('disabled')) {
                return;
            }

            var $btns = $wrapper.find('.ajax_add_to_cart'),
                queue = [];

            function endQueue() {
                $btns.removeClass('disabled');
                $btn.removeClass('disabled');

                $btn.one('click', onAddAllClicked);
                $wrapper.unblock();
            }

            function nextQueue() {
                if (queue.length === 0) {
                    endQueue();
                    return;
                }

                var $current = $(queue.splice(0, 1));
                $body.one('added_to_cart', nextQueue);
                $current.trigger('click');
            }

            function startQueue() {
                $wrapper.block();
                queue = $btns;
                $btn.addClass('disabled');
                $btns.addClass('disabled');
                nextQueue();
            }

            startQueue();
        }

        if ($btn.length) {
            $btn.one('click', onAddAllClicked);
        }
    }

    $(window).load(function () {
        $('.handheld-header-search > a').click(function (e) {
            $(this).parent().toggleClass('active');
            e.preventDefault();
            $('.handheld-header-search .search-field').focus();
        });

    });

    /* FUNÇÃO RESPONSÁVEL PELO <ENTER> NO CALCULADOR DE FRETE */
    $(window).load(function () {
        var input = document.getElementById("zipcode");
        var btCalcularFrete = document.getElementsByClassName("calculate")[0];
        $(window).keydown(function (event) {
            if (document.activeElement === input) {
                if (event.keyCode === 13) {
                    event.preventDefault();
                    btCalcularFrete.click();
                    return false;
                }
            }
        });

        /* MUDANÇA DO NOME "CRAYON" DO EDITOR DE CÓDIGO-FONTE */
        var a = document.getElementById("qt_bbp_topic_content_crayon_quicktag");
        var b = document.getElementById("qt_comment_crayon_quicktag");
        if (a) {
            if (a.value == "crayon") {
                while (a.value == "crayon") {
                    a.value = "CODE";
                }
            }
        }
        if (b) {
            if (b.value == "crayon") {
                while (b.value == "crayon") {
                    b.value = "CODE";
                }
            }
        }
    });
    
    $('#calc_shipping_postcode').keypress(function(event){
        var keycode = (event.keyCode ? event.keyCode : event.which);

        if($('#calc_shipping_postcode').focus() && keycode == 13){
            $('.shipping-calculator-form button[name=\'calc_shipping\']').click();
        }
    });

    if ($('.maker-hero__makers-slide').length) {
        var slider = $('.maker-hero__makers-slide').slick({
            infinite: true,
            speed: 300,
            slidesToShow: 5,
            slidesToScroll: 1,
            centerMode: false,
            variableWidth: true,
            adaptiveHeight: true
        });

        $('.maker-hero__makers-slide-maker [data-larn-more]').on('click', function (e) {
            e.preventDefault();

            var $el = $(e.target).parent();
            $el.toggleClass('maker-hero__makers-slide-maker--open');
        });
    }


    $('[data-toggle="tooltip"]').tooltip();

    $('.home-page .dropdown-menu.yamm').hover( function(){
        $('body').addClass('dark-ff');
    }, function () {
        $('body').removeClass('dark-ff');
    });

    $('.departments-menu-v2-title').click(function () {
        $('body').addClass('dark-ff').fadeIn('slow');
    });

    $(document).click(function () {

        var $body = $('body');

        if($body.hasClass('dark-ff')) {
            $body.removeClass('dark-ff').fadeIn('slow');
        }
    });

    if ($('.showcase__slide-items').length) {
        $('.showcase__slide-items').slick({
            dots: true,
            arrows: false,
            infinite: true,
            autoplay: true,
            autoplaySpeed: 4000,
            speed: 1000,
            fade: true,
            cssEase: 'linear'
        });
    }

    $('#billing_number').removeAttr('autocomplete');
    $('#billing_number').attr('type', 'search');

    try{
        $('#pagarme-installments').select2();

        $('#email_status_order').select2();
    } catch (ignored) {}
    detectAddAllMaterial();

})(jQuery, window, document);