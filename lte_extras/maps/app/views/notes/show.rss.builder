xml.instruct!

xml.rss("version" => "2.0",
        "xmlns:geo" => "http://www.w3.org/2003/01/geo/wgs84_pos#",
        "xmlns:georss" => "http://www.georss.org/georss") do
  xml.channel do
    xml.title t("notes.rss.title")
    xml.description t("notes.rss.description_item", :id => @note.id)
    xml.link url_for(:controller => "site", :action => "index", :only_path => false)

    xml << render(:partial => "note", :object => @note)
  end
end
