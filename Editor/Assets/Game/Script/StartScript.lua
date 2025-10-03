function Start()
end

function Update(deltaTime)
end

function OnCollisionEnter(other)
	if(other:compareTag("Player")) then 
        local audio = gameObject:getAudioComponent()
        audio:play()
	end
end 