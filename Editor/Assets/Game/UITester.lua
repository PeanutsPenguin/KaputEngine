function start()
    local text = getUIObject("omg")
    local newText = text:castToUIText()

    print(newText.name)

    local value = newText.text
    print(value)

    newText.text = "omg new value"

    print(newText.text)
end


function update(deltatime)
end