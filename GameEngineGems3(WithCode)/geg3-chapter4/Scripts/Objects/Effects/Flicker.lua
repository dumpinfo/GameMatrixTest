Script.frequency=200--int "Frequency" 0 10000
Script.variation=0.25--float "Variation" 0 1
Script.strength=0.5--float "Strength" 0 1
Script.Recursive=true--bool
Script.nextupdatetime=0
Script.state=false

function Script:Start()
	self:SaveIntensity(self.entity,self.Recursive)
end

--Store the original intensity values
function Script:SaveIntensity(entity,recursive)
	entity:SetKeyValue("Flicker_Intensity",entity:GetIntensity())
	if recursive then
		local n
		for n=0,entity:CountChildren()-1 do
			self:SaveIntensity(entity:GetChild(n),true)
		end
	end
end

--Apply a modified intensity value
function Script:ApplyIntensity(entity,intensity,recursive)
	local i = entity:GetKeyValue("Flicker_Intensity")
	if i~="" then
		entity:SetIntensity(tonumber(i) * intensity)
		if recursive then
			local n
			for n=0,entity:CountChildren()-1 do
				self:ApplyIntensity(entity:GetChild(n),intensity,true)
			end
		end
	end
end

function Script:Draw()
	if self.color==nil then
		self.color = self.entity:GetColor()
	end
	
	local t = Time:GetCurrent()
	
	if t>self.nextupdatetime then
		self.nextupdatetime=t + self.frequency + math.random(-self.frequency*self.variation,self.frequency*self.variation)
	
		--Update the object
		self.state = not self.state
		if self.state then
			self:ApplyIntensity(self.entity,1.0,self.Recursive)
		else
			self:ApplyIntensity(self.entity,1.0-self.strength,self.Recursive)
		end

	end
end
