require 'genx'

function testOutput(d)
    d:startElement('document')

    d:startElement('author')
    d:attribute('awesome', 'true')
    d:text('Adrian')
    d:endElement()

    d:startElement('body')
    d:text('Hello, everybody')
    d:comment('how are you?')
    d:endElement()

    d:endElement()
end

-- file output to stdout
doc = genx.new(io.stdout)
testOutput(doc)
print ''

-- sender output
doc = genx.new(io.write)
testOutput(doc)
print ''